#include "InvertedIndexProcessor.hh"
#include "log4class.hh"
#include <math.h>
  
namespace nycpp
{
    InvertedIndexProcessor::InvertedIndexProcessor(vector<WebPage>& pageList, unordered_map<string, unordered_map<PageId, double>>& invertedIndexTable)
        :_pagesList(pageList), _invertedIndexTable(invertedIndexTable)
        {}
    
    void InvertedIndexProcessor::process()
    {

        _sumWeightofWords.resize(_pagesList.size(), 0);
        //computer TF(term frequency of each word in each page)
        for(auto& page : _pagesList)
        {

            auto& wordsMap = page.getWordMap();
            // title is a word (eg. std::vector)
            const string& title = page.getTitle();
            

            int numOfWords = wordsMap.size(); 
            
            if(numOfWords == 0){
                logError("This file has no word. : %s", page.getTitle());
                continue;
            }

            for(auto& wordAndFreq : wordsMap)
            {
                string word = wordAndFreq.first;
            
                double TF =  static_cast<double>(wordAndFreq.second);
                _invertedIndexTable[word].insert(std::make_pair(page.getDocId(), TF));
            }
        }

        int N = _pagesList.size();
        //computer IDF
        for(auto& wordAndPageIdTF : _invertedIndexTable)
        {
            // < word, map<docId, TF> >
            auto& pageIdAndTfMap = wordAndPageIdTF.second;
            
            for(auto& pageIdAndTF : pageIdAndTfMap)
            {
                size_t pageId =  pageIdAndTF.first;
                double TF = pageIdAndTF.second;
                int DF = pageIdAndTfMap.size();
                
                double IDF =  log2( N /static_cast<double>(DF + 1));
                double weight = TF * IDF;

                pageIdAndTF.second = weight;
                _sumWeightofWords[pageId] += (weight*weight);                
            }
        }

        for(auto& word_PageIdAndIDFMap : _invertedIndexTable)
        {
            auto& pageIdAndIDFMap = word_PageIdAndIDFMap.second;

            for(auto& pageIdAndIDF : pageIdAndIDFMap)
            {
                size_t pageId = pageIdAndIDF.first;
                double sumOfWeight = _sumWeightofWords[pageId];
               
                if(sumOfWeight == 0)
                {
                    logError("file :%d  sum of weight is zero.", pageId);
                    exit(EXIT_FAILURE);
                }
                
                pageIdAndIDF.second /= sqrt(sumOfWeight); 
            }
        }                
    }
};//name space


