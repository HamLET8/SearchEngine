#include "OffsetProcessor.hh"
#include "log4class.hh"
#include "WebPage.hh"


namespace nycpp
{
    OffsetProcessor::OffsetProcessor(vector<WebPage>& pages, vector<pair<size_t, size_t>>& offsetLib)
        :_pages(pages), _offsetLib(offsetLib)
    {}

    void OffsetProcessor::process()
    {
        logDebug("offset lib is creating.");
        
        size_t offset = 0;
        for(auto& page : _pages)
        {
            size_t textLength = page.getDoc().size();
            _offsetLib.push_back(std::make_pair(offset, textLength));
            offset += textLength;
        }
    }
};