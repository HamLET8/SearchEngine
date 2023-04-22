#include "Configuration.hh"
#include "log4class.hh"

#include <sstream>
#include <fstream>
#include "Dictionary.hh"


namespace nycpp
{
    Dictionary* Dictionary::_pSingletonDic = nullptr;
    std::mutex Dictionary::_mutex;

    Dictionary* Dictionary::getInstance()
    {

        if(_pSingletonDic == nullptr)
        {
            _mutex.lock();
            if(_pSingletonDic == nullptr)
            {
                _pSingletonDic = new Dictionary();
                _pSingletonDic->initDict();
                _pSingletonDic->initIndex();
                atexit(destory);
            }
            _mutex.unlock();
        }
        return _pSingletonDic;
    }

    void Dictionary::destory()
    {
        if(_pSingletonDic != nullptr)
        {
            delete _pSingletonDic;
            _pSingletonDic = nullptr;
        }
    }

    void Dictionary::initDict()
    {
        logInfo("int dictionary.");
        string dictPath = Configuration::getConfigInstance()->getConfigMap()["dict"];

        std::ifstream ifs(dictPath);
        if(!ifs.good())
        {
            logError("open dictionary file : %s failed", dictPath.c_str());
            exit(EXIT_FAILURE);   
        }

        string line; // word and number

        while(getline(ifs, line))
        {
            std::istringstream iss(line);
            string word, freq;
            iss >> word >> freq;
            _dict.push_back(std::make_pair(word, std::stoi(freq)));
        }
    }
    
    
    void Dictionary::initIndex()
    {
        logInfo("init dictionary Indexed Table class.");

        string dicIndexPath = Configuration::getConfigInstance()->getConfigMap()["dicIndex"];

        std::ifstream dicIfs(dicIndexPath);
        if(!dicIfs.good())
        {
            logError("open dictionary Index Table file failed.");
            exit(EXIT_FAILURE);
        }

        string line;

        while(getline(dicIfs, line))
        {
            std::istringstream iss(line);

            string word, docId;
            iss >> word;

            while(iss >> docId)
            {
                _indexedTable[word].insert(docId == "" ? 0 : std::stoi(docId));
            }
        }
    }

    const Dictionary::Dict& Dictionary::getDict()
    {
        return _dict;
    } 

    const Dictionary::IndexTable& Dictionary::getIndexTable()
    {
        return _indexedTable;
    }
}