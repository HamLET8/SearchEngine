#include "WebPageSearcher.hh"
#include "Configuration.hh"
#include "log4class.hh"
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"
#include <fstream>

#include "MultiBytesCharacter.hh"
using namespace nlohmann;

template <typename K, typename V, typename dummy_compare, typename alloc>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, alloc>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

#include <fstream>

namespace nycpp
{
    using std::pair;
    using std::ifstream;

    WebPageSearcher::WebPageSearcher()
    {
        loadLibrary();
        loadInvertIndexTable();
    }

    WebPageSearcher::~WebPageSearcher()
    {
    }

    void WebPageSearcher::loadLibrary()
    {
        string stopWordFilePath = Configuration::getConfigInstance()->getConfigMap()["stopwords"];

        ifstream stopWordIfs(stopWordFilePath);
        if(!stopWordIfs.good())
        {
            logError("open stop word file failed!");
            exit(EXIT_FAILURE);
        }

        string stopWord;
        while(stopWordIfs >> stopWord)
        {
            _stopWordList.push_back(stopWord);
        }


        string pageLibPath = Configuration::getConfigInstance()->getConfigMap()["ripepage"];
        ifstream pageLibIfs(pageLibPath);
        if(!pageLibIfs.good())
        {
            logError("open page library file : ripepage.dat failed!");
            exit(EXIT_FAILURE);
        }

        string offsetLibPath = Configuration::getConfigInstance()->getConfigMap()["offset"];
        ifstream offsetLibIfs(offsetLibPath);
        if(!offsetLibIfs.good())
        {
            logError("open offset library file : offset.dat failed!");
            exit(EXIT_FAILURE);
        }

        // doc_id, doc_start , doc_length
        string docOffsetLine;
        PageId docId;
        size_t docBeg, offset;

        // doc_id, doc_title, doc_url, doc_content 
        while(getline(offsetLibIfs, docOffsetLine))
        {
            std::stringstream ss(docOffsetLine);
            ss >> docId >> docBeg >> offset;
            _offsetLib[docId] = std::make_pair(docBeg, offset);

            vector<char> buf(offset);
            pageLibIfs.read(buf.data(), offset);


            WebPage temp(string(buf));
            _pageLib.emplace_back(string(buf.data(), offset)); 
        }

        stopWordIfs.close();
        pageLibIfs.close();
        offsetLibIfs.close();   
    }

    void WebPageSearcher::loadInvertIndexTable()
    {
        string invertIndexTablePath = Configuration::getConfigInstance()->getConfigMap()["invertIndex"];
        ifstream invertIndexTableIfs(invertIndexTablePath);
        if(!invertIndexTableIfs.good())
        {
            logError("open invert index table file : invertindex.dat failed!");
            exit(EXIT_FAILURE);
        }

        string line;
        while(getline(invertIndexTableIfs, line))
        {
            std::stringstream ss(line);
            string word;
            int docId;
            double weight;
            ss >> word ;
            while(ss)
            {
                ss >> docId >> weight;
                _invertIndexTable[word].insert(std::make_pair(docId, weight));
            }
        }

        invertIndexTableIfs.close();
    }
    
    string WebPageSearcher::doQuery(const string& queryWord)
    {
        logInfo("query word: %s ", queryWord.c_str());
        WebPage pageIn;
        pageIn.setPageContent(queryWord);
        pageIn.splitWord(_splitTool, _stopWordList);
    
        unordered_map<string, double> vecX = getQueryVec(pageIn);
        set<PageId> docIdSet = getDocIdSet(pageIn);

        if(docIdSet.empty())
        {
            logError("no result found!");
            return serializeNull();
        }

        DocVector sortedPageId = sortResult(docIdSet, vecX);

        vector<PageId> idSet;
        for(auto& id : sortedPageId)
        {
            idSet.push_back(id.second);
        }

        setSummary(idSet, pageIn);
        
        string response = serializeResult(sortedPageId);

        return response;
    }




    unordered_map<string, double> WebPageSearcher::getQueryVec(WebPage& page)
    {
        unordered_map<string, double> docIdVec;
        const unordered_map<string, int>& wordFreqMap = page.getWordMap();
        
        // calculate weight of each word : weight = TF * IDF 
        for(auto& wordFreq : wordFreqMap)
        {
            string word = wordFreq.first;
            int freq = wordFreq.second;
            double weight = 0.0;
            if(_invertIndexTable.find(word) != _invertIndexTable.end())
            {
                weight = log(_pageLib.size() / (_invertIndexTable[word].size() + 1)) * freq;
            }
            docIdVec[word] = weight;
        }

        // normalize the vector : w = w / sqrt(w1^2 + w2^2 + ... + wn^2) 
        double sum = 0.0;

        for(auto& wordWeight : docIdVec)
        {
            sum += wordWeight.second * wordWeight.second;
        }
        sum = sqrt(sum);

        for(auto& wordWeight : docIdVec)
        {
            wordWeight.second = wordWeight.second / sum;
        }

        return docIdVec;
    }

    set<PageId> WebPageSearcher::getDocIdSet(const WebPage& PageIn)
    {

        vector<set<PageId>> docIdSetVec;

        // get docIdSet of each word in query
        const unordered_map<string, int>& wordFreqMap = PageIn.getWordMap();
        for(auto& wordFreq : wordFreqMap)
        {
            set<PageId> docIdSet;
            string word = wordFreq.first;
            if(_invertIndexTable.find(word) != _invertIndexTable.end())
            {
                for(auto& docIdWeight : _invertIndexTable[word])
                {
                    docIdSet.insert(docIdWeight.first);
                }
            }

            docIdSetVec.push_back(docIdSet);
        }
        
        if(docIdSetVec.empty())
        {
            return set<PageId>();
        }

        // get the intersection of docIdSet
        set<PageId> docIdSet = docIdSetVec[0];
        for(size_t i = 1; i < docIdSetVec.size(); ++i)
        {
            set<PageId> temp;
            set_intersection(docIdSet.begin(), docIdSet.end(), docIdSetVec[i].begin(), docIdSetVec[i].end(), inserter(temp, temp.begin()));
            docIdSet = temp;
        }

        return docIdSet;        
    }

    using DocVector = std::vector<std::pair<double, size_t>>;

    DocVector WebPageSearcher::sortResult(set<PageId>& docIdSet, unordered_map<string, double>& queryVec)
    {

        // get the vector of each document
        vector<vector<double>> docVecVec;

        for(auto& docId : docIdSet)
        {
            vector<double> docVec;
            for(auto& wordWeight : queryVec)
            {
                string word = wordWeight.first;
                double weight = wordWeight.second;
                auto& docIdSet = _invertIndexTable[word];
                for(auto docIdWeight : docIdSet)
                {
                    if(docIdWeight.first == docId)
                    {
                        docVec.push_back(docIdWeight.second);
                        break;
                    }
                }
            }
            docVecVec.push_back(docVec);
        }

        vector<double> queryWeightVec;
        for(auto& wordWeight : queryVec)
        {
            queryWeightVec.push_back(wordWeight.second);
        }

        //calculate the similarity between query vector and each document vector
        //similarity is calculated by the formula : (vecX * vecY) / (|vecX| * |vecY|)
        DocVector simVec;
        auto iter = docIdSet.begin();
        for(size_t i = 0; i < docVecVec.size(); ++i)
        {
            double similarity = 0.0;
            double sumX = 0.0;
            double sumY = 0.0;
            for(size_t j = 0; j < docVecVec[i].size(); ++j)
            {
                similarity += docVecVec[i][j] * queryWeightVec[j]; //x * y
                sumX += docVecVec[i][j] * docVecVec[i][j];  //x^2
                sumY += queryWeightVec[j] * queryWeightVec[j];          //y^2
            }
            sumX = sqrt(sumX);  //sqrt(x^2)
            sumY = sqrt(sumY);  //sqrt(y^2)
            similarity = similarity / (sumX * sumY);   // cos 
            simVec.push_back(std::make_pair(similarity, *iter++));
        }

        //sort the result by similarity
        sort(simVec.begin(), simVec.end(), std::greater<pair<double, PageId>>());

        return simVec;
    }

void WebPageSearcher::setSummary(vector<PageId> &sortedIDs, WebPage &pageX)
{
    const size_t STEP = 40;                                      // 目标字符待往左/右偏移的字符数
    const unordered_map<string, int> &wordsMapX = pageX.getWordMap(); // <word, freq>
    vector<PageId> tmpIDs = sortedIDs;

    for (auto &ID : tmpIDs) // 依次设置所有网页的摘要
    {
        static int n = 0;
        cout << ++n << endl;

        const string content = _pageLib[ID].getContent();

        size_t first_pos = SIZE_MAX;     // page 中第一次出现 wordsMapX 中的单词的位置
        for (auto &wordPair : wordsMapX) // pair<string, int> wordPair
        {
            string word = wordPair.first;
            size_t pos = content.find(word);
            // if (pos != string::npos && pos < first_pos)
            if (pos != content.npos && pos < first_pos)
                first_pos = pos;
        }

        if (first_pos == SIZE_MAX) // 这篇文章中的 content 部分，没有 wordsMapX 中的单词，而在 title 部分有 wordsMapX 中的单词。此时该篇文章应被剔除
        {
            remove(sortedIDs.begin(), sortedIDs.end(), ID);
            continue;
        }

        size_t first_to_end = content.substr(first_pos).size();                                             // 从 content[first_pos] 到字符串末尾所占字节数
        size_t right_pos = first_pos + howManyBytesWithNCharacter(&content[first_pos], first_to_end, STEP); // 从 content[first_pos] 到其后 STEP 个字符所占字节数（first_to_end 为上限）

        vector<size_t> ppc = getPosPerCharactor(content, first_pos);       // 从字符串前 first_pos 字节中所有字符的起始字节
        size_t left_pos = ppc.size() >= STEP ? ppc[ppc.size() - STEP] : 0; 
        // size_t left_pos = ppc.size() >= STEP ? ppc[ppc.size() - STEP] : ppc[0]; // ppc[0] 一定是 0

        string summary = "";
        if (left_pos != 0) // content[left_pos] 前还有字符
            summary += " ... ";
        summary += content.substr(left_pos, right_pos - left_pos);
        if (right_pos < content.size()) // content[right_pos] 后还有字符
            summary += " ... ";
        // summary += content.substr(first_pos);
        _pageLib[ID].setPageSummary(summary);
    }
}




    string WebPageSearcher::serializeResult(DocVector& sortedPageId)
    {
        string result;
        const size_t recommMax = std::stoi(Configuration::getConfigInstance()->getConfigMap()["maxpagenum"]);

        Json root;
        root["msgID"] = 200;

        Json msg;
        PageId cnt = 0;

        for(auto& docId : sortedPageId)
        {
            PageId id = docId.second;
            if(++cnt > recommMax)
            {
                break;
            }
            WebPage& sendPage = _pageLib[id];

            Json file;
            file["title"] = sendPage.getTitle();
            file["url"] = sendPage.getUrl();
            file["summary"] = sendPage.getSummary();
            msg.push_back(file);
        }

        root["msg"] = msg;

        return root.dump(4, ' ', false, Json::error_handler_t::ignore);
    }

    string WebPageSearcher::serializeNull()
    {
        Json root;
        root["msgID"] = 404;
        root["msg"] = json("No related file exist.");

        return root.dump(4);
    }



}//end of namespace nycpp