#pragma once
#include "DirScanner.hh"
#include "WebPage.hh"
#include "PageProcesser.hh"
#include "InvertedIndexProcessor.hh"
#include "OffsetProcessor.hh"

namespace nycpp
{
    class PageLib
    {
    public:
        explicit PageLib(const string&);

        void createLib();
        void storeLib();

    private:
        vector<WebPage> _pages;
        vector<pair<size_t, size_t>> _offsetTable;
        unordered_map<string, unordered_map<PageId, double>> _invertedIndexTable;

        // scan dir path to find files
        DirScanner _dirScanner;

        // reduce  reduandency 
        PageProcessor _pageProcessor;

        // compute inverted index table
        InvertedIndexProcessor _invertedIndexProcessor;
        
        // computer offset table
        OffsetProcessor _offsetProcessor;
    };
};