#include "SplitTool.hh"
#include "DictProducer.hh"
#include "Configuration.hh"
#include "log4class.hh"

using namespace nycpp;

int main()
{
    string cnCorpusPath = Configuration::getConfigInstance()->getConfigMap()["cnDir"];
    string enCorpusPath = Configuration::getConfigInstance()->getConfigMap()["enDir"];

    SplitTool tool;

    DictProducer enDict(enCorpusPath);
    DictProducer cnDict(cnCorpusPath, &tool);

    return 0;
}

