#pragma once
#include <string>
#include "NonCopyable.hh"

namespace nycpp
{
    class Socket;
    class InetAddress;

    class Acceptor : private NonCopyable
    {
    public:
        explicit Acceptor(const std::string&, unsigned short);

        void prepare();
        int accept();
        int serverFd();

        ~Acceptor();
    
    private:
        void setReuseAddr(bool);
        void setReusePort(bool);
        void bind();
        void listen();
    
    private:
        Socket* _listenSock;
        InetAddress* _serverAddr;
    };
}