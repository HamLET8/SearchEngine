#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include <mutex>

namespace nycpp
{
    using std::map;
    using std::pair;
    using std::string;
    using std::vector;


    class Dictionary
    {

    public:
        using Dict = std::vector<pair<string, int>>; // <word, freq> 
        using IndexTable = std::map<std::string, std::set<int>>;

    public:
        static Dictionary* getInstance();
        static void destory();


        const Dict& getDict();
        const IndexTable& getIndexTable();

    private:
        Dictionary(){};
        ~Dictionary(){};

        void initDict();
        void initIndex();
        
    private:        
        static Dictionary* _pSingletonDic;
        Dict _dict;
        IndexTable _indexedTable;
        static std::mutex _mutex;
    };

};