#include "LRUCache.hh"
#include "log4class.hh"
#include "Thread.hh"

#include <fstream>
#include <sstream>


extern __thread size_t __thread_id;
namespace nycpp
{
    LRUCache::LRUCache(size_t capacity)
        :_capacity(capacity)
    { }

    bool LRUCache::isHited(const string& query)
    {
        std::cout << query << std::endl;
        if(_cacheMap.find(query) != _cacheMap.end())
        {
            return true;
        } 
    }

    string LRUCache::getRecord(const string& query)
    {
        if(_cacheMap.find(query) == _cacheMap.end())
        {
            return string();
        }

        _resultList.splice(_resultList.begin(), _resultList, _cacheMap[query]);
        return _cacheMap[query]->second;        
    }

    void LRUCache::setRecord(const string& query, const string& result)
    {
        if(_cacheMap.find(query) != _cacheMap.end())
        {
            _resultList.splice(_resultList.begin(), _resultList, _cacheMap[query]);
            return;
        }

        _resultList.emplace_front(query, result);
        
        _cacheMap[query] = _resultList.begin();
        if(_cacheMap.size() > _capacity)
        {
            auto last = _resultList.back().first;
            _resultList.pop_back();
            _cacheMap.erase(last);
        }
    }

    void LRUCache::clear()
    {
        _resultList.clear();
        _cacheMap.clear();
    }

    void LRUCache::update(const LRUCache& cache)
    {
        for(auto it = cache._resultList.rbegin(); it != cache._resultList.rend(); ++it)
        {
            setRecord(it->first, it->second);
        }
    }

    size_t LRUCache::size() const
    {
        return _resultList.size();
    }
}
