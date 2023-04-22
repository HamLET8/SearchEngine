#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "RssParser.hh"
#include "SplitTool.hh"

namespace nycpp
{
    using PageId = size_t; 
    using std::unordered_map;

    class WebPage
    {
    public:
        WebPage() = default;
        explicit WebPage(const string&);
        explicit WebPage(const RssItem&);

        string getDoc() const;
        PageId getDocId() const;
        string getTitle() const;
        string getUrl() const;
        string getContent() const;
        string getSummary() const;
        const unordered_map<string, int>& getWordMap() const; 

        void setPageId(PageId);
        void setPageDoc();
        void setPageContent(const string&);
        void setPageSummary(const string&);

        void splitWord(SplitTool&, const vector<string>&);
        //vector<string> calcTopK(int topK);
    private:
        string _doc;
        PageId _docId;
        string _docTitle;
        string _docURL;
        string _docContent;
        string _docSummary;
        //vector<string> _topKword;
        unordered_map<string, int> _wordMap;
    };

};