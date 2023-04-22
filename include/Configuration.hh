#pragma once

#include <stdlib.h>
#include <string>
#include <unordered_map>

namespace nycpp
{
    using std::string;
    using std::unordered_map;

    class Configuration{
    public:
        static Configuration* getConfigInstance();
        unordered_map<string, string>& getConfigMap();

    private:
        Configuration(const string& filepath);
        ~Configuration(){};

        void loadConf();
        static void destroy();

    private:
        string _filePath;
        unordered_map<string, string> _configMap;
        static Configuration* _pInstance;

    };

};