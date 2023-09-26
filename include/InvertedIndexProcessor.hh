#pragma once

#include "WebPage.hh"

#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <utility>

namespace nycpp
{
    using std::unordered_map;
    using std::vector;
    class InvertedIndexProcessor{
    
    public:

        InvertedIndexProcessor(vector<WebPage>&, unordered_map<string, unordered_map<PageId, double>>&);

        void process();

    private:
        vector<WebPage>& _pagesList;    
        unordered_map<string, unordered_map<PageId, double>>& _invertedIndexTable;
        vector<double> _sumWeightofWords;
    };


};