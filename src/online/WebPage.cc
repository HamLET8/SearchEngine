#include "WebPage.hh"
#include "log4class.hh"

#include <sstream>
#include <regex>


namespace nycpp
{
    using std::regex;

    WebPage::WebPage(const string& doc)
        :_doc(doc)
    {

        size_t pos = doc.find("<docid>");
        size_t pos2 = doc.find("</docid>");

        _docId = std::stoi(doc.substr(pos + 7, pos2 - pos - 7));

        pos = doc.find("<url>");
        pos2 = doc.find("</url>");

        _docURL = doc.substr(pos + 5, pos2 - pos - 5);

        pos = doc.find("<title>");
        pos2 = doc.find("</title>");

        _docTitle = doc.substr(pos + 7, pos2 - pos - 7);

        pos = doc.find("<content>");
        pos2 = doc.find("</content>");

        _docContent = doc.substr(pos + 11, pos2 - pos - 11);

        
    }

    WebPage::WebPage(const RssItem& item)
        :_docId(0)
    {
        _docTitle = item.title;
        _docURL = item.link;
        _docContent = item.description;
    }

    string WebPage::getDoc() const
    {
        return _doc;
    }

    PageId WebPage::getDocId() const
    {
        return _docId;
    }

    string WebPage::getTitle() const
    {
        return _docTitle;
    }

    string WebPage::getUrl() const
    {
        return _docURL;
    }

    string WebPage::getContent() const
    {
        return _docContent;
    }

    string WebPage::getSummary() const
    {
        return _docSummary;
    }

    const unordered_map<string, int>& WebPage::getWordMap() const 
    {
        return _wordMap;
    }

    void WebPage::setPageDoc()
    {
        _doc = 
           "<doc>\n\t<docid>" + std::to_string(_docId) +
           "</docid>\n\t<title>" + _docTitle +
           "</title>\n\t<url>" + _docURL +
           "</url>\n\t<content>" + _docContent +
           "</content>\n</doc>\n";
    }

    void WebPage::setPageId(PageId id)
    {
        _docId = id;
    }

    void WebPage::setPageContent(const string& content)
    {
        _docContent = content;
    }

    void WebPage::setPageSummary(const string& summmary)
    {
        _docSummary = summmary;
    }

    void WebPage::splitWord(SplitTool& tool, const vector<string>& stopWords)
    {
        auto words = tool.cut(_docTitle + _docContent);
        
        for(auto& word : *words)
        {
            if(word != " " && (find(stopWords.begin(), stopWords.end(), word) == stopWords.end()))
            {
                ++_wordMap[word];
            }
        }

    }

    

    




};

