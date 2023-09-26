#include "Configuration.hh"
#include "log4class.hh"
#include <iostream>
#include <fstream>
#include <sstream>

extern const char* ConfigPath;
namespace nycpp
{
    using std::ifstream;
    using std::stringstream;

    Configuration* Configuration::_pInstance = nullptr;
    
    Configuration::Configuration(const string& filePath)
    :_filePath(filePath)
    {
        loadConf();
    }

    void Configuration::loadConf()
    {
        ifstream ifs(_filePath, ifstream::in);
        logInfo("open config file.");

        if(!ifs.good())
        {
            logError("open config file failed! ");
        }

        string lines;
        string key;
        string value;

        while(getline(ifs, lines))
        {
            stringstream ss(lines);
            ss >> key >> value;
            _configMap[key] = value;
            logInfo("get config key and value");
        }

        ifs.close();    
    }

    Configuration* Configuration::getConfigInstance()
    {
        if(_pInstance == nullptr)
        {
            logInfo("create Configuration object.");
            string configFilePath(ConfigPath);
            _pInstance = new Configuration(configFilePath);
            atexit(destroy);
        }

        return _pInstance;
    }

    void Configuration::destroy()
    {
        logInfo(" configuration object call destory. ");
        if(_pInstance)
        {
            delete _pInstance;
            _pInstance = nullptr;
        }
    }

    unordered_map<string, string>& Configuration::getConfigMap()
    {
        return _configMap;
    }

} //nycpp


