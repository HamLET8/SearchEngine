#include "CacheManager.hh"
#include "Configuration.hh"
#include "log4class.hh"

namespace nycpp
{
    CacheManager* CacheManager::_pInstance = CacheManager::getInstance();

    CacheManager* CacheManager::getInstance()
    {
        if(_pInstance == nullptr)
        {
            auto configMap = Configuration::getConfigInstance()->getConfigMap();

            size_t cacheGroupNum = std::stoul(configMap["workernum"]);
            size_t maxLRUCapacity = std::stoul(configMap["recordnum"]);

            _pInstance = new CacheManager(cacheGroupNum, maxLRUCapacity);
            atexit(destory);
        }
        return _pInstance;
    }

    void CacheManager::destory()
    {
        if(_pInstance)
        {
            delete _pInstance;
        }
    }

    CacheGroup& CacheManager::getCacheGroup(size_t index)
    {
        return _cacheGroups[index];
    }

    CacheManager::CacheManager(size_t cacheGroupsNum, size_t maxRecord)
        :_cacheNum(cacheGroupsNum), 
         _maxRecord(maxRecord),
         _cacheGroups(cacheGroupsNum, maxRecord)
        {
            logInfo("CacheManager created. CacheGrop : %d . Max LRU Capacity: %d ", _cacheNum, _maxRecord);
        }




    void CacheManager::sync()
    {
        //first update  head Cache, then use the updated head cache to update other cache.
        using namespace std;
        logInfo("Cache start synchronize. ");

        auto& cacheHead = _cacheGroups[0];
        cacheHead._readOnly = true;

        for(auto& cache : _cacheGroups)
        {
            auto& pendingUpdateCache = cache._pendingUpdateCache;
            cacheHead._mainCache.update(pendingUpdateCache);

            pendingUpdateCache.clear();
        }

        for(auto& groupCache : _cacheGroups)
        {
            groupCache._readOnly = true;
            groupCache.update(cacheHead);
            groupCache._readOnly = false;
        }

        cacheHead._readOnly = false;    
    }
    

}


