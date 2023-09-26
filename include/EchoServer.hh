#pragma once

#include "ThreadPool.hh"
#include "TcpServer.hh"
#include "WebPageSearcher.hh"
#include "TimerThread.hh"
#include "KeyRecommander.hh"

#include <sw/redis++/redis++.h>
#include "TcpServer.hh"

using namespace sw::redis;

namespace nycpp
{
    // EchoServer is a server that echo the message it received
    class EchoServer
    {
    public:
        EchoServer(const string &ip, unsigned short port);

        void start();
        void stop();

        void onConnection(const TcpConnectionPtr &);
        void onMessage(const TcpConnectionPtr &);
        void onClose(const  TcpConnectionPtr &);

    private:
        const size_t INIT_WORKER_NUM = 5;
        const size_t INIT_TASKQUEUE_CAPACITY = 10;

        ThreadPool _threadPool;
        TcpServer _tcpServer;
        WebPageSearcher _webPageSearcher;
        KeyRecommander _keyRecommander;
        sw::redis::Redis _redis;
        TimerThread _timerThread;
    };
}


