#pragma once
#include "LRUCache.hh"

namespace nycpp
{
    class CacheManager;

    class CacheGroup
    {
        friend class CacheManager;

    public:
        CacheGroup(size_t capacity);
        ~CacheGroup() = default;

        string getRecord(const string& query);
        void   setRecord(const string& query, const string& value);

        void update(CacheGroup& other);
    
    private:
        LRUCache _mainCache;
        LRUCache _pendingUpdateCache;

        bool _readOnly;
    };

}

