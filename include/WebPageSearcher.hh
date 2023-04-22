#pragma once
#include "WebPage.hh"
#include "SplitTool.hh"

#include <unordered_map>

namespace nycpp
{
    using std::set;

    class WebPageSearcher
    {
    public:
        using DocVector = std::vector<std::pair<double, size_t>>;

    public: 
        WebPageSearcher();
        ~WebPageSearcher();

        string doQuery(const string& queryWord);
    
    private:
        void loadLibrary();
        void loadInvertIndexTable();
        vector<string> splitSentence(const string& content);
        unordered_map<string, double>   getQueryVec(WebPage& page);
        set<PageId>                     getDocIdSet(const WebPage& pageIn);
        DocVector                       sortResult(set<PageId>& docIdSet, unordered_map<string, double>& queryVec);
        void                            setSummary(vector<PageId> &sortedIDs, WebPage &pageX);
        DocVector                       getSortedPageId(unordered_map<string, double>& vecX, set<PageId>& docIdSet);
    
        string                          serializeResult(DocVector& sortedPageId); 
        string                          serializeNull(); 
    private:
        vector<WebPage> _pageLib;
        unordered_map<int, std::pair<int, int>> _offsetLib;
        unordered_map<string, std::set<std::pair<size_t, double>>> _invertIndexTable;

        SplitTool _splitTool;
        vector<string> _stopWordList; 
    };



}