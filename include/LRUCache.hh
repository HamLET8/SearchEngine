#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <utility>
#include <boost/locale.hpp>


namespace nycpp
{
    using std::string;
    using std::unordered_map;
    using std::list;
    using std::pair;

    class CacheManager;

    class LRUCache
    {
    public:
        friend class CacheManager;
        using Record = pair<string, string>;

    public:                
        LRUCache(size_t capacity);
        ~LRUCache() = default;

        bool isHited(const string&);
        string getRecord(const string&);
        void setRecord(const string&, const string&);

        void update(const LRUCache&);
        void clear();

        size_t size() const;   
    
    private:
        list<Record> _resultList;
        unordered_map<string, list<Record>::iterator> _cacheMap;
        size_t _capacity;
        
    };

}