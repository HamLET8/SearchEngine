#include "KeyRecommander.hh"
#include "Dictionary.hh"
#include "Configuration.hh"
#include "log4class.hh"
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"

#include <set>
#include <algorithm>

namespace nycpp
{


using namespace nlohmann;
template<typename K, typename V, typename dummy_compare, typename A>
using my_workaround_fifomap = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifomap>;
using Json = my_json;



string KeyRecommander::query(const string& queWord)
{
    Dictionary* pdic = Dictionary::getInstance();

    //map< string, set<int> >  : word , doc_id
    const Dictionary::IndexTable& indexTable = pdic->getIndexTable();

    std::set<int> indexSet;
    string index;

    for(size_t idx = 0; idx < queWord.length(); ++idx) 
    {
        size_t nBytes  = nBytesCode(queWord[idx]); // UTF encode
        index = queWord.substr(idx, nBytes);

        idx += (nBytes - 1 );
        auto it = indexTable.find(index);
        // it :  doc id set which contain queried word
        if(it != indexTable.end())
        {
            indexSet.insert(it->second.begin(), it->second.end());
        }
    }

    if(indexSet.size() == 0)
    {
        logError("Key Recommander miss %s .", queWord.c_str());
        return serializeNull();
    }

    PriorQueResult resultQue;
    statistic(queWord, indexSet, resultQue);
    vector<string> result;

    const int maxCandicate = stoi(Configuration::getConfigInstance()->getConfigMap()["maxkeynum"]);

    for(int i = 0; i < maxCandicate ; ++i)
    {
        if(resultQue.empty() == false)
        {
            result.push_back(resultQue.top().getWord());
            resultQue.pop();
        }
    }

    return serialize(result);
}


void KeyRecommander::statistic(const string& queryWord, std::set<int>& indexId, PriorQueResult& resultQue)
{
    // word frequecy
    auto& dict = Dictionary::getInstance()->getDict();

    //*it : doc id which contain query word
    for(auto it = indexId.begin(); it != indexId.end(); ++it)
    {
        QResult res(dict[*it].first, dict[*it].second);
        int dist = distance(queryWord, dict[*it].first);

        res.setDistance(dist);
        resultQue.push(res);
    }
   
}


size_t KeyRecommander::length(const string& str)
{
    size_t len = 0;
    for(size_t idx = 0; idx != str.size(); ++idx)
    {
        int nByte = nBytesCode(str[idx]);
        idx += (nByte - 1);
        ++len;
    }

    return len;

}

int KeyRecommander::distance(const string& lhs, const string& rhs)
{
    if(lhs == rhs) return 0;

    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);

    int editDist[lhs_len + 1][rhs_len + 1];
    for(size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }

    for(size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }

    string sublhs, subrhs;

    for(size_t i = 1, lhs_idx = 0; i < lhs_len; ++i, ++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1 );

        for(size_t j = 1, rhs_idx = 0; j < rhs_len; ++j, ++rhs_idx)
        {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);

            if(sublhs == subrhs)
            {
                editDist[i][j] = editDist[i-1][j-1];
            }
            else
            {
                editDist[i][j] = std::min({editDist[i-1][j], editDist[i][j-1], editDist[i-1][j-1]})+ 1;
            }

        }
    }
    return editDist[lhs_len][rhs_len];
}



string KeyRecommander::serializeNull()
{
    Json root;
    root["msgID"] = 404;
    root["msg"] = json("no word to recommand.");

    return root.dump(4);
}

string KeyRecommander::serialize(const vector<string>& result)
{
    Json root;
    root["msgID"] = 100;

    Json msg = result;
    root["msg"] = msg;

    return root.dump(4);
}



size_t KeyRecommander::nBytesCode(const char ch)
{
   if(ch & (1 << 7))
   {
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx)
        {
            if(ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }else
                break;
        }
        return nBytes; 
   } 
   return 1;
}



    
} 
