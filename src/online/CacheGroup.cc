#include "CacheGroup.hh"

namespace nycpp
{
    CacheGroup::CacheGroup(size_t capacity)
        :_mainCache(capacity), _pendingUpdateCache(capacity), _readOnly(false)
    { }

    string CacheGroup::getRecord(const string& query)
    {
        return _mainCache.getRecord(query);
    }

    void CacheGroup::setRecord(const string& query, const string& result)
    {
        _mainCache.setRecord(query, result);
        if(_readOnly == true)
        {
            return ;
        }
        _pendingUpdateCache.setRecord(query, result);
    }


    void CacheGroup::update(CacheGroup& other)
    {
        if(&other == this)
        {
            return ;
        }
        _mainCache.update(other._mainCache);
        _pendingUpdateCache.update(other._pendingUpdateCache);
    }


}