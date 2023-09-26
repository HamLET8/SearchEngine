#include "DictProducer.hh"
#include "Configuration.hh"
#include "log4class.hh"
#include "SplitTool.hh"

#include <memory>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>


namespace nycpp
{
    using std::ifstream;
    using std::istringstream;

    DictProducer::DictProducer(const string& dir)
        :_enDir(dir)
    {
        string enDictPath = Configuration::getConfigInstance()->getConfigMap()["enDict"];
        string enDictIndex = Configuration::getConfigInstance()->getConfigMap()["enDicIndex"];
        string enStopDictPath = Configuration::getConfigInstance()->getConfigMap()["enStop"];

        loadStopWord(enStopDictPath);
        
        buildEnDict();
        storeDict(enDictPath);

        createIndex();
        storeIndex(enDictIndex);

        logInfo("creating english dictionary.");
    }

    DictProducer::DictProducer(const string& dir, SplitTool* splitTool)
    :_cnDir(dir), _splitTool(splitTool)
    {
        string cnDictPath = Configuration::getConfigInstance()->getConfigMap()["dict"];
        string cnDictIndex = Configuration::getConfigInstance()->getConfigMap()["dicIndex"];
        string cnStopDictPath = Configuration::getConfigInstance()->getConfigMap()["cnStop"];
        
        logInfo("load chinese stop word.");
        loadStopWord(cnStopDictPath);

        logInfo("build chinese dictionary.");
        buildCnDict();
        storeDict(cnDictPath);

        logInfo("build chinese Index.");
        createIndex();
        storeIndex(cnDictIndex);
    }

    void DictProducer::loadStopWord(const string& stopWordFilePath)
    {
        _stopWordSet.clear();
        ifstream ifs(stopWordFilePath);
        if(!ifs.good())
        {
            logError("open stop word file failed.");
            return;
        }

        string line;
        string word;

        while(getline(ifs, line))
        {
            istringstream iss(line);
            while(iss >> word)
            {
                _stopWordSet.insert(word);
            }            
        }

        ifs.close();
    }

    //open all file, and get all word, and count frequency.
    void DictProducer::buildEnDict()
    {
        std::stringstream ss;
        string line;
        string word;
        string keyWord;

        _files.clear();

        logInfo("get all files under current english dict directory.");
        getFiles(_enDir);

        for(auto& file : _files)
        {
            ifstream ifs(file);
            if(!ifs.good())
            {
                logError("open english dictionary file failed.");
                return;
            }


            while(getline(ifs, line))
            {
                istringstream iss(line);
                while(iss >> word)
                {
                    keyWord.clear();
                    for(auto& c : word)
                    {
                        if(isalpha(c))
                        {
                            keyWord += tolower(c);
                        }else{
                            break;
                        }
                    }

                    if(keyWord != " " && find(_stopWordSet.begin(), _stopWordSet.end(), keyWord) != _stopWordSet.end() )
                    {
                        ++_dictW[keyWord];
                    }else{
                        continue;
                    }                   
                }
            }

            ifs.close();
        }
    }

    void DictProducer::buildCnDict()
    {
        getFiles(_cnDir);

        for(auto& file : _files)
        {
            //open file with tail.
            ifstream ifs(file, std::ios::ate);

            if(!ifs.good())
            {
                logError("open chinese dictionay file failed.");
                return;
            }

            size_t fileLength = ifs.tellg();
            ifs.seekg(std::ios::beg);
            
            string buff(fileLength + 1, '\0');
            ifs.read(&buff[0], fileLength);

            //std::unique_ptr<char[]> buffer(new char[fileLength+1]());
            
            //ifs.read(buffer.get(), fileLength + 1);

            //string text(buffer.get());

            std::shared_ptr<vector<string>> wordCut = _splitTool->cut(buff);

            for(auto& keyWord: *wordCut)
            {
                //if not stop word, and is chinese word.
                if(!_stopWordSet.count(keyWord) && getNumByte_UTF8(keyWord[0]) == 3)
                {
                    if(_dictW.find(keyWord) != _dictW.end())
                    {
                        ++_dictW[keyWord];
                    }else{
                        _dictW[keyWord] = 1;
                    }
                }
            }

            ifs.close();
        }
    }

    void DictProducer::createIndex()
    {
        // every word has own id, increase by 1. 
        int nodeId = 0; 
        for(auto& elem : _dictW)
        {
            string word = elem.first;
            
            //cn or en char
            size_t charN = word.size() / getNumByte_UTF8(word[0]);
            for(size_t idx = 0,n = 0; n != charN; ++idx, ++n )
            {
                size_t charLen = getNumByte_UTF8(word[idx]);
                string subWord = word.substr(idx, charLen);

                //each char insert current nodeId to its index table. 
                _index[subWord].insert(nodeId);
                idx += (charLen -1);
            }
            ++nodeId;
        }
    }

    void DictProducer::storeIndex(const string& filePath)
    {
        std::ofstream ofs(filePath);
        if(!ofs.good())
        {
            logError("create index file failed. ");
            return;
        }

        for(auto& charIdx : _index)
        {
            ofs << charIdx.first << " ";
            for(auto& idx : charIdx.second)
            {
                ofs << idx << " ";
            }
            ofs << std::endl;
        }
        ofs.close();
    }

    void DictProducer::storeDict(const string& filePath)
    {
        std::ofstream ofs(filePath);
        if(!ofs.good())
        {
            logError("create dict file failed. ");
            return;
        }

        for(auto& wordFreq : _dictW)
        {
            // word and frequency
            ofs << wordFreq.first << " " << wordFreq.second <<  std::endl;
        }
        ofs.close();
    }

    void DictProducer::getFiles(const string& dir)
    {
        DIR* dirp = opendir(dir.c_str());   
        if(dirp == nullptr)
        {
            logError("open dir failed.");
            return ;
        }

       struct dirent* pdirent;
       while((pdirent = readdir(dirp)) != nullptr)
       {
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
            {
                continue;
            }
            string filePath = dir + "/" + pdirent->d_name;

            _files.push_back(filePath);
       }
       closedir(dirp); 
    }

    // check num bit that represent char
    size_t DictProducer::getNumByte_UTF8(const char byte)
    {
        int bNum = 0;
        for(size_t i = 0; i < 6; ++i)
        {
            if(byte & (1 << (7-i)))
            {
                ++bNum;
            }
            else break;
        }
        return (bNum == 0) ? 1 : bNum; 
    }

    void DictProducer::showFiles() const
    {
        for(auto &elem : _files)
        {
            std::cout << elem << std::endl;
        }
    }

    void DictProducer::showDict() const
    {
        for(auto& it : _index)
        {
            std::cout << it.first << " ";
            for(auto& set_it : it.second)
            {
                std::cout << set_it << " ";
            }
        }
    }

};