#include "WebPage.hh"
#include "log4class.hh"
#include "PageLib.hh"
#include "Configuration.hh"

#include <iostream>
#include <string>

int main(int argc, const char** argv) {
    
    std::string pageFilesPath = nycpp::Configuration::getConfigInstance()->getConfigMap()["pages"];

    logDebug("create PageLib object, parameter : filePath : %s ", pageFilesPath.c_str());
    nycpp::PageLib lib(pageFilesPath);

    logDebug("call PageLib->createLib().");
    lib.createLib();
    
    logDebug("call PageLib->storeLib().");
    lib.storeLib();

    return 0;
}