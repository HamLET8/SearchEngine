#pragma once
#include <string>
#include <vector>

namespace nycpp
{
    using std::string;
    using std::vector;

    class DirScanner
    {
    public:
        explicit DirScanner(const string&);

        void traverse();
        vector<string>& getFilesPath();

    private:
        void traverse(const string&);

    private:
        vector<string> _filesPath;
        string _dirPath;
    };

};