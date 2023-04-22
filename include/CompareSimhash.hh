#pragma once
#include "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/Simhasher.hpp"
#include "WebPage.hh"

#include <vector>

namespace nycpp{
    using std::vector;

    class HashNode
    {
    private:

        friend class CompareSimhash;

        explicit HashNode(uint64_t hashVal)
        {
            _keyA = hashVal & (0xffff000000000000);
            _keyB = hashVal & (0x0000ffff00000000);
            _keyC = hashVal & (0x00000000ffff0000);
            _keyD = hashVal & (0x000000000000ffff);
            _listA.push_back(hashVal);
            _listB.push_back(hashVal);
            _listC.push_back(hashVal);
            _listD.push_back(hashVal);
        }
    
    private:
        uint64_t _keyA;
        uint64_t _keyB;
        uint64_t _keyC;
        uint64_t _keyD;

        vector<uint64_t> _listA;
        vector<uint64_t> _listB;
        vector<uint64_t> _listC;
        vector<uint64_t> _listD;
    };

    class CompareSimhash
    {
    private:
        const string DICT_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/jieba.dict.utf8";
        const string MODEL_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/hmm_model.utf8";
        const string IDF_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/idf.utf8";
        const string STOP_WORDS_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/stop_words.utf8";

    public:
        CompareSimhash()
            :_simHasher(DICT_PATH, MODEL_PATH, IDF_PATH, STOP_WORDS_PATH)
        {}

        bool same(const WebPage&);


    private:
        vector<HashNode> _hashList;
        const size_t _topN = 10;
        simhash::Simhasher _simHasher;
    };


}
