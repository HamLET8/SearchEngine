#pragma once

#include "tinyxml2.h"

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <memory>

namespace nycpp
{
    using std::cin;
    using std::cout;
    using std::endl;

    using std::string;
    using std::vector;

    struct RssItem   
    {
    public:
        bool checkValid()
        {
            if(title == "" || link == "" || description == "") return true;

            if(title == description) return true;

            if(description.size() <= title.size()) return true;

            for(auto& s : description)
            {
                if(s != ' ' || s != '\n' || s!= '\t' ) return false;
            }

            return true; 
        }

    public:
        string title;
        string link;
        string description;
        string content;    
    };

    class RssParser
    {
        public:
            explicit RssParser();
            void dump(const string&);
            std::shared_ptr<vector<RssItem>> getRssItems() const;
            void parse(const char*);
        
        private:
            string dissolve(string);

        private:
            std::shared_ptr<vector<RssItem>> _pRss;
    };

};