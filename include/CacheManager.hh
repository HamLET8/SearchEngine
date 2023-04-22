#pragma once
#include "CacheGroup.hh"

#include <vector>

namespace nycpp
{
    using std::vector;

    class CacheManager
    {
    public:
        static CacheManager* getInstance();
        CacheGroup& getCacheGroup(size_t index);

        void sync();

    private:
        CacheManager(size_t cacheGroups, size_t maxRecord);
        ~CacheManager(){}

        static void destory();

    private:
        size_t _cacheNum;
        size_t _maxRecord;

        vector<CacheGroup> _cacheGroups;
        static CacheManager* _pInstance;
    };

}
