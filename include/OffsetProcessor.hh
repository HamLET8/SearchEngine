#pragma once

#include "WebPage.hh"
#include <vector>
#include <map>

namespace nycpp
{
    using std::pair;
    
    class OffsetProcessor{
    public:
        OffsetProcessor(vector<WebPage>&, vector<pair<size_t, size_t>>&);
        void process();

    private:
        vector<WebPage>& _pages;
        vector<pair<size_t, size_t>>& _offsetLib;
    };    
} // namespace nycpp


