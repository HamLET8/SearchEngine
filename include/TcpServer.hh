#pragma once
#include "Acceptor.hh"
#include "EventLoop.hh"
#include <functional>

namespace nycpp
{
    class TcpServer
    {
    public:
        using TcpServerCallBack = std::function<void(const TcpConnectionPtr &)>;
    
    public:
        TcpServer(const string&, unsigned short); // < ip ,port >
        void start();
        void stop();

        void setConnectionCallBack(TcpServerCallBack&&);
        void setMessageCallBack(TcpServerCallBack&&);
        void setCloseCallBack(TcpServerCallBack&&);

    private:
        Acceptor _acceptor;
        EventLoop _loop;       
    };
}