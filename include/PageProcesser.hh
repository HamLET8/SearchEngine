#pragma once
#include "CompareSimhash.hh"
#include "SplitTool.hh"
#include "WebPage.hh"

#include <iostream>
#include <vector>
#include <string>

namespace nycpp
{
    class PageProcessor
    {
    public:
        
        explicit PageProcessor(vector<string>&, vector<WebPage>&);

        void process();
        void printPageList();
        void printStopWords();

    
    private:
        void loadStopWords();
        void loadPageFromXML();
        void cutRedundantPage();
        void coutFrequence();

    private:

        vector<string>& _filesPath;
        vector<WebPage>& _pages;
        vector<string> _stopWords;

        CompareSimhash _compareSim;
        SplitTool _splitTool;

    };
}
