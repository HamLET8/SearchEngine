#include "Acceptor.hh"
#include "Socket.hh"
#include "InetAddress.hh"
#include "ErrorCheck.hh"
#include "log4class.hh"

namespace nycpp
{
    Acceptor::Acceptor(const string& ip, unsigned short port)
        :_listenSock(new Socket()), _serverAddr(new InetAddress(ip, port))
    {
        logInfo("Acceptor( %s , %d)", ip, port);
    }

    void Acceptor::prepare()
    {
        setReuseAddr(true);
        setReusePort(true);
        
        logDebug("bind");
        bind();
        
        logDebug("listen");
        listen();
    }

    void Acceptor::setReuseAddr(bool flag)
    {
        int opt = static_cast<int>(flag);
        int ret = ::setsockopt(_listenSock->getFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        ERROR_CHECK(ret, -1, "socketopt reuse addr");
    }

    void Acceptor::setReusePort(bool flag)
    {
        int opt = static_cast<int>(flag);
        int ret = ::setsockopt(_listenSock->getFd(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        ERROR_CHECK(ret, -1, "setsockopt reused port");
    }

    void Acceptor::bind()
    {
        int ret = ::bind(_listenSock->getFd(), 
                        (struct sockaddr *)(_serverAddr->GetInetAddress()),
                        sizeof(struct sockaddr)
                        );
        ERROR_CHECK(ret, -1, "bind");
    }

    void Acceptor::listen()
    {
        int ret = ::listen(_listenSock->getFd(), 65535);
        ERROR_CHECK(ret, -1, "listen");
    }

    int Acceptor::accept()
    {
        logInfo("accept");
        int clientFd = ::accept(_listenSock->getFd(),nullptr, nullptr);
        ERROR_CHECK(clientFd, -1, "accept");
        return clientFd;
    }

    int Acceptor::serverFd()
    {
        return _listenSock->getFd();
    }

    Acceptor::~Acceptor()
    {
        logInfo("~Acceptor");
        delete _listenSock;
        delete _serverAddr;
    }
}