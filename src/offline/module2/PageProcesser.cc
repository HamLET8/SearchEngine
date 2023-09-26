#include "PageProcesser.hh"
#include "RssParser.hh"
#include "log4class.hh"
#include "Configuration.hh"

#include <chrono>
#include <algorithm>
#include <fstream>

namespace nycpp
{
    PageProcessor::PageProcessor(vector<string>& filesPath, vector<WebPage>& pages)
        :_filesPath(filesPath), _pages(pages)
        {
            loadStopWords();
        }

    void PageProcessor::loadStopWords()
    {
        std::ifstream ifs(Configuration::getConfigInstance()->getConfigMap()["stopwords"]);

        if(!ifs.good())
        {
            logError("load stop words failed. ");
            exit(EXIT_FAILURE);
        }
        
        string word;

        while(ifs >> word)
        {
            _stopWords.push_back(word);
        }
    }    


    void PageProcessor::process()
    {
        {
            logDebug("load web page from xml doc.");

            auto start = std::chrono::system_clock::now();
            loadPageFromXML();
            auto end = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            std::cout << "load xml page costs time:" << duration.count() << std::endl;
        }

        {
            logDebug("cut reduandent pages.");
            auto start = std::chrono::system_clock::now();
            cutRedundantPage();
            auto end = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            std::cout << "cut reduandent pages costs time:" << duration.count() << std::endl;
        }

        {
            logDebug("count word frequency.");
            auto start = std::chrono::system_clock::now();
            coutFrequence();
            auto end = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            std::cout << "count frequency costs time:" << duration.count() << std::endl;
        }

        int idx = 0;
        for(auto& page : _pages)
        {
            page.setPageId(idx++);
            page.setPageDoc();
        }
    }

    void PageProcessor::loadPageFromXML()
    {
        PageId id = 0;
        RssParser rssParser;
        
        for(auto& file : _filesPath)
        {
            rssParser.parse(file.c_str());
        }

        logDebug("parse xml file finished.");

        for(auto& item : *rssParser.getRssItems())
        {
           WebPage page(item);
           page.setPageId(id++);
           _pages.push_back(std::move(page));
        }
    }

    void PageProcessor::cutRedundantPage()
    {
        int idx = 0;
        for(vector<WebPage>::iterator it = _pages.begin(); it != _pages.end();)
        {
            if(_compareSim.same(*it))
            {
                it = _pages.erase(it);
            }else{
                ++it;
            }
        }
    }

    void PageProcessor::coutFrequence()
    {
        for(auto& page : _pages)
        {
            page.splitWord(_splitTool, _stopWords);
        }
    }

    void PageProcessor::printStopWords()
    {
        return;
    }

    void PageProcessor::printPageList()
    {
        return;
    }
};

