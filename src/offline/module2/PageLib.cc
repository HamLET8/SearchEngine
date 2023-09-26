#include "PageLib.hh"
#include "log4class.hh"
#include "Configuration.hh"

#include <fstream>
using std::fstream;

namespace nycpp
{
    PageLib::PageLib(const string& dirPath)
        :_dirScanner(dirPath),
         _pageProcessor(_dirScanner.getFilesPath(), _pages),
         _invertedIndexProcessor(_pages, _invertedIndexTable),
         _offsetProcessor(_pages, _offsetTable)
         {}
    

    void PageLib::createLib()
    {
        logDebug("call pageProcessor to page formatting.");
        _pageProcessor.process();

        logDebug("call inveredIndexProcessor to create inverted index table.");
        _invertedIndexProcessor.process();

        logDebug("call offsetProcessor to create offset Table.");
        _offsetProcessor.process();
    }

    void PageLib::storeLib()
    {
        string ripePagePath = Configuration::getConfigInstance()->getConfigMap()["ripepage"];
        string offsetPath = Configuration::getConfigInstance()->getConfigMap()["offset"];
        string invertedTablePath = Configuration::getConfigInstance()->getConfigMap()["invertIndex"];


        logInfo("start write ripe page to disk.");
        std::ofstream ripeOfs(ripePagePath);
        if(!ripeOfs.good())
        {
            logError("create ripe page file failed.");
            exit(EXIT_FAILURE);
        }

        for(auto& page : _pages)
        {
            ripeOfs << page.getDoc();
        }
        ripeOfs.close();
        logInfo("writed ripe page finished.");


        logInfo("Start write offset table to disk.");
        std::ofstream offsetOfs(offsetPath);
        if(!offsetOfs.good())
        {
            logError("create offset file failed.");
            exit(EXIT_FAILURE);
        }

        for(size_t idx = 0; idx < _offsetTable.size() && idx < _pages.size(); ++idx)
        {
            offsetOfs   << _pages[idx].getDocId()   << "\t"
                        << _offsetTable[idx].first  << "\t" 
                        << _offsetTable[idx].second << "\t"
                        << std::endl;
        }        
        offsetOfs.close();
        logInfo("writed offset table finished.");


        logInfo("Start write inverted index table to disk.");
        std::ofstream indexTableOfs(invertedTablePath);
        if(!indexTableOfs.good())
        {
            logError("create inverted index table file failed.");
            exit(EXIT_FAILURE);
        }

        //wordP : < word, <docId, weightOfWord> >
        for(auto& wordP : _invertedIndexTable )
        {
            indexTableOfs << wordP.first << " ";

            for(auto& pageIdAndW : wordP.second)
            {
                indexTableOfs   << pageIdAndW.first  << " "
                                << pageIdAndW.second << " ";
            }
            indexTableOfs << std::endl;
        }
               
        indexTableOfs.close();
        logInfo("writed inverted index table finished.");

    }
    
};
