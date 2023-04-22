#include "EchoServer.hh"
#include "WebPageSearcher.hh"
#include "MyTask.hh"
#include "log4class.hh"
#include "TimerTask.hh"
#include "Configuration.hh"
#include <iostream>
#include "Acceptor.hh"
#include "ThreadPool.hh"

namespace nycpp
{

    EchoServer::EchoServer(const string &ip, unsigned short port)
        :_threadPool(INIT_WORKER_NUM, INIT_TASKQUEUE_CAPACITY),
         _tcpServer(ip, port),
         _webPageSearcher(),
         _keyRecommander(),
         _redis("tcp://127.0.0.1:6379"),
         _timerThread(std::bind(&TimerTask::process, TimerTask()),
                        std::stoi(Configuration::getConfigInstance()->getConfigMap()["initTime"]),
                        std::stoi(Configuration::getConfigInstance()->getConfigMap()["periodicTime"])
                        )
    { }

    void EchoServer::start()
    {
        _threadPool.start();
        _timerThread.start();

        using namespace std::placeholders;
        _tcpServer.setConnectionCallBack(std::bind(&EchoServer::onConnection, this, _1));
        _tcpServer.setMessageCallBack(std::bind(&EchoServer::onMessage, this, _1));
        _tcpServer.setCloseCallBack(std::bind(&EchoServer::onClose, this, _1));

        _tcpServer.start();  
    }

    void EchoServer::stop()
    {
        _tcpServer.stop();
        
        _timerThread.stop();

        _threadPool.stop();
    }

    void EchoServer::onConnection(const TcpConnectionPtr& connPtr)
    {
        logInfo("%s connected", connPtr->show().c_str());
        std::cout << connPtr->show() << "connected." << std::endl;
    }

    void EchoServer::onMessage(const TcpConnectionPtr& connPtr)
    {
        logInfo("Message");

        string msg = connPtr->recv();  // train : len / msg
        std::shared_ptr<MyTask> task = std::make_shared<MyTask>(msg, connPtr, _webPageSearcher, _keyRecommander, _redis);

        _threadPool.addTask(std::bind(&MyTask::process, task));
    }

    void EchoServer::onClose(const TcpConnectionPtr& connPtr)
    {
        logInfo("%s disconnected.", connPtr->show().c_str());

        std::cout << connPtr->show() << "connected." << std::endl;
    }


}
