#include "RssParser.hh"
#include "log4class.hh"
#include "tinyxml2.h"
#include <regex>

namespace nycpp
{
    RssParser::RssParser()
    :_pRss(new vector<RssItem>())
    {
        logDebug("RssParser constructor.");
    }

    void RssParser::parse(const char* fileName)
    {
        logDebug("start parse  %s", fileName);
        tinyxml2::XMLDocument doc;
        if(doc.LoadFile(fileName))
        {
            logError("load file %s failed.", fileName);
            doc.PrintError();
            exit(EXIT_FAILURE);   
        }

        tinyxml2::XMLElement* channel = doc.FirstChildElement("rss")->FirstChildElement("channel");
        RssItem home;
        home.title = channel->FirstChildElement("title")->GetText();
        home.link = channel->FirstChildElement("link")->GetText();
        home.description = channel->FirstChildElement("description")->GetText();

        if(!home.checkValid())
        {
            _pRss->push_back(home);
        }

        tinyxml2::XMLElement *item = channel->FirstChildElement("item");

        while(item)
        {
            RssItem page;
            page.title = item->FirstChildElement("title")->GetText();
            page.link = item->FirstChildElement("link")->GetText();
            page.description = item->FirstChildElement("description")->GetText();

            page.description = dissolve(page.description);

            if(!page.checkValid())
            {
                _pRss->push_back(page);
            }
            item = item->NextSiblingElement("item");
        }
    } 
    
    
    void RssParser::dump(const string& fileName)
    {
        std::ofstream ofs(fileName);
        if(!ofs.good())
        {
            logError("save file %s failed.", fileName);
            exit(EXIT_FAILURE);
        }
        for(size_t i = 0; i < _pRss->size(); ++i)
        {
            ofs << "doc" << endl
            << "\t<docod>" <<  i << "</docid>" << endl
            << "\t<title>" << (*_pRss)[i].title << "</title>" << endl
            << "\t<link>"  << (*_pRss)[i].link  << "</link>"  << endl
            << "\t<description>" << (*_pRss)[i].description << "</description>" << endl
            << "\t<content>" << (*_pRss)[i].content << "</content>" << endl
            << "</doc>" << endl;
        }
    }


    std::shared_ptr<vector<RssItem>> RssParser::getRssItems() const
    {
        return _pRss;
    }
    

    string RssParser::dissolve(string page)
    {
        string text = page;
        {
            std::regex pattern("<(.[^>]*)>");
            text = regex_replace(text, pattern, "");
        }
        {
            std::regex pattern("showPlayer\\(\\{[^\\}]+\\}\\);");
            text = regex_replace(text, pattern, "");
        }
        {
            std::regex pattern("&nbsp;");
            text = regex_replace(text, pattern, "");
        }
        return text;
    }
};
