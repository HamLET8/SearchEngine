#include "EchoServer.hh"
#include "Configuration.hh"
#include "CacheManager.hh"
#include <iostream>
#include <string>
#include "WebPageSearcher.hh"
using namespace nycpp;

void test1()
{
    const string ip = nycpp::Configuration::getConfigInstance()->getConfigMap()["ip"];

    const unsigned short port = std::stoi(nycpp::Configuration::getConfigInstance()->getConfigMap()["port"]);

    EchoServer server(ip, port);
    server.start();

    while(1)
    {
        std::string cmd;
        std::cin >> cmd;
        if(cmd == "quit")
        {
            server.stop();
            break;
        }
    }
}


void test2()
{
    auto p = CacheManager::getInstance();
    auto cacheGroup = p->getCacheGroup(1);

    cacheGroup.getRecord("hello");

    cacheGroup.setRecord("1", "one");
    cacheGroup.setRecord("2", "two");
    cacheGroup.setRecord("5", "three");
    cacheGroup.setRecord("4", "four");

    auto res = cacheGroup.getRecord("1");
    std::cout << res << std::endl;

}

int main()
{ 
    test1();
    return 0;
}
