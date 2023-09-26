#include "Configuration.hh"
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"

using namespace nlohmann;
template <typename k, typename v, typename dummy_compare, typename alloc>
using my_workaround_fifo_map = fifo_map<k, v, fifo_map_compare<k>, alloc>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;
#include "log4class.hh"
#include "ErrorCheck.hh"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

const char* ConfigPath = "/home/jingbo-l/cplus/SearchEngine/conf/myconf.conf";
using namespace std;
int serverFd;

// this is a search engine client menu
void showMenu()
{
    cout << "**********Welcome to Search Engine**********" << endl;
    cout << "**********1. Query word **********" << endl;
    cout << "**********2. Web page search **********" << endl;
    cout << "**********3. Exit **********" << endl;
    cout << "**********Please input your choice:**********" << endl;
}

// search result page slpit display
void shoWithPaging(Json &root)
{
    size_t pageNum = root["msg"].size();
    size_t pageSize = 5;

    size_t totalPage = pageNum / pageSize;

    if(pageNum == 0)
    {
        cout << "No result found" << endl;
        return;
    }

    if(pageNum == 1)
    {
        cout << "1 page in total" << endl;
    }
    else
    {
        cout << pageNum << " pages in total" << endl;
    }

    size_t idx = 0;
    for( ; idx < 5 && idx < pageNum; ++idx)
    {
        auto &item = root["msg"][idx];

        cout << "[title]: " << item["title"] << endl;
        cout << "[url]: " << item["url"] << endl;
        cout << "[summary]: " << item["summary"] << endl;
        cout << endl;
    }

    while(idx < pageNum)
    {
        cout << "**********Please input 1 to continue**********" << endl;
        int choice;
        cin >> choice;
        if(choice == 1)
        {
            for(size_t i = 0; i < 5 && idx < pageNum ; ++i)
            {
                auto &item = root["msg"][idx++];

                cout << "[title]: " << item["title"] << endl;
                cout << "[url]: " << item["url"] << endl;
                cout << "[summary]: " << item["summary"] << endl;
                cout << endl;
            }
        }
        else
        {
            cout << "**********Input error**********" << endl;
            return;
        }
    }
}

    size_t sendm(const void* buf, size_t len)
    {
        int left = len;
        const char* ptr = (const char*)buf;
        int ret = 0;

        while(left > 0)
        {
            do
            {
                ret = send(serverFd, ptr, left, 0);
            }while(ret == -1 && errno == EINTR);

        
            if(ret == -1)
            {
                perror("send");
                return len - ret;
            }
            else if(ret == 0)
            {
                logError("send : Server closed");
                return left;
            }
            else
            {
                left -= ret;
                ptr += ret;
            }   
        }
        return len;
    }


    size_t recvm(void* buf, size_t len)
    {
        int left = len;
        char* ptr = (char*)buf;
        int ret = 0;

        while(left > 0)
        {
            do
            {
                ret = recv(serverFd, ptr, left, 0);
            }while(ret == -1 && errno == EINTR);

            if(ret == -1)
            {
                perror("recv");
                return len - ret;
            }
            else if(ret == 0)
            {
                logError("recv : Server closed");
                return left;
            }
            else
            {
                left -= ret;
                ptr += ret;
            }
        }
    }

    void sendKey(string& key)
    {
        Json root;
        root["msgID"] = 1;
        root["msg"] = key;
        string jsonStr = root.dump(4);

        size_t len = jsonStr.size();
        sendm(&len, sizeof(len));
        sendm(jsonStr.c_str(), len);

    }


    void sendQuery(string& page)
    {
        Json root;
        root["msgID"] = 2;
        root["msg"] = page;
        string jsonStr = root.dump(4);
        cout << "jsonStr = " << jsonStr << endl;

        size_t len = jsonStr.size();
        sendm(&len, sizeof(len));
        sendm(jsonStr.c_str(), len);
    }

    void recvKey()
    {
        size_t len;
        recvm(&len, sizeof(len));
        cout << "msg len = " << len << endl;
        char buf[len + 1] = {0};
        recvm(buf, len);

        string msg(buf);
        //debug
        cout << "msg = " << msg << endl;

        Json root = json::parse(msg);
        if(100 == root["msgID"])
        {
            logInfo("recvive key from server.");
            cout << "key: " << root["msg"] << endl;

        }
        else if(404 == root["msgID"])
        {
            logInfo("recv 404 from server when key searching.");
            cout << root["msg"] << endl;
        }
        else
        {
            logError("message recive error.");
            cout << "error happened when recv key." << endl;
            close(serverFd);
            exit(EXIT_FAILURE);
        }        
    }

    void recvWebPages()
    {
        size_t len = 0;
        recvm(&len, sizeof(len));

        char buf[len + 1] = {0};
        recvm(buf, len);

        string msg(buf);

        Json root = json::parse(msg);
        
        if(200 == root["msgID"])
        {
            logInfo("recv pages from server.");
            shoWithPaging(root);        
        }
        else if(404 == root["msgID"])
        {
            logInfo("recv 404 from server when page searching.");
            cout << root["msg"] << endl;
        }
        else
        {
            cout << "error when recv page from server." << endl;
            logError("error when page searching.");
            close(serverFd);
            exit(EXIT_FAILURE);
        }
    }


    int main(int argc, char* argv[])
    {
        using namespace nycpp;

        string ip = Configuration::getConfigInstance()->getConfigMap()["ip"];
        string port = Configuration::getConfigInstance()->getConfigMap()["port"];

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(atoi(port.c_str()));

        serverFd = socket(AF_INET, SOCK_STREAM, 0);
        ERROR_CHECK(serverFd, -1, "socket");

        int ret = connect(serverFd, (struct sockaddr*)&addr, sizeof(addr));
        ERROR_CHECK(ret, -1, "connect");

        while(1)
        {
            showMenu();

            int choice;
            cin >> choice;

            string msg;

            switch (choice)
            {
            case 1: 
                cout << "please input a key: " << endl;
                cin >> msg;
                sendKey(msg);
                recvKey();
                break;
            
            
            case 2:
                cout << "Please input your query:" << endl;
                cin >> msg;
                sendQuery(msg);
                recvWebPages();
                break;

            case 3:

                close(serverFd);
                exit(EXIT_SUCCESS);
                
            default:
               
                cout << "Cant recongize! input again :" << endl; 
                exit(EXIT_FAILURE);
            }

        }
        close(serverFd);
        return 0;
    }





