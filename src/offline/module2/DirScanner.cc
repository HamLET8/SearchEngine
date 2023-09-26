#include "DirScanner.hh"
#include "log4class.hh"

#include <sys/types.h>
#include <dirent.h>
#include <cstring>

namespace nycpp
{
    DirScanner::DirScanner(const string& dirPath)
        :_dirPath(dirPath)
    {
        traverse();
    }


    void DirScanner::traverse()
    {
        traverse(_dirPath);
    }

    void DirScanner::traverse(const string& dirName)
    {
        DIR* fdir = opendir(dirName.c_str());
        if(fdir == nullptr)
        {
            logError("open file %s failed.", dirName.c_str());
            exit(EXIT_FAILURE);
        }

        struct dirent* pDirent;
        while(nullptr != (pDirent = readdir(fdir)))
        {
            if(strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            {
                continue;
            }

            if(pDirent->d_type == 8)
            {
                string filePath(dirName + "/" + pDirent->d_name);
                _filesPath.push_back(filePath);
            }

            if(pDirent->d_type == 4)
            {
                string subDirPath = dirName + "/" + pDirent->d_name;
                traverse(subDirPath);
            }
        }
    }

    vector<string>& DirScanner::getFilesPath()
    {
        return _filesPath;
    }



} // namespace nycpp
