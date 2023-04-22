#pragma once
#include "../3rdparty/simhash-cppjieba/cppjieba/Jieba.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nycpp
{
    using std::string;
    class SplitTool
    {
    private:
        const string DICT_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/jieba.dict.utf8";
        const string HMM_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/hmm_model.utf8";
        const string USER_DICT_PATH = "/home/jingbo-l/cplus/projectThread/3rdparty/simhash-cppjieba/dict/user.dict.utf8";
    public:
        SplitTool();
        std::shared_ptr<std::vector<string>> cut(const string&);
    
    private:
        cppjieba::Jieba _jieba;
    }; 

}; // namespace nycpp
