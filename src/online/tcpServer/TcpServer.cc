#include "TcpServer.hh"

namespace nycpp
{
    TcpServer::TcpServer(const string& ip, unsigned short port)
        :_acceptor(ip, port), _loop(_acceptor)
    { }

    void TcpServer::start()
    {
        _acceptor.prepare();
        _loop.loop();
    }

    void TcpServer::stop()
    {
        _loop.unloop();
    }

    void TcpServer::setConnectionCallBack(TcpServerCallBack&& onConnectCb)
    {
        _loop.setConnectionCallBack(std::move(onConnectCb));
    }

    void TcpServer::setMessageCallBack(TcpServerCallBack&& onMessageCb)
    {
        _loop.setMessageCallBack(std::move(onMessageCb));
    }

    void TcpServer::setCloseCallBack(TcpServerCallBack&& onCloseCb)
    {
        _loop.setCloseCallBack(std::move(onCloseCb));
    }
    
}