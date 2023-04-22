#pragma once
#include "Socket.hh"
#include "SocketIO.hh"
#include "InetAddress.hh"
#include "NonCopyable.hh"
#include <memory>
#include <functional>

namespace nycpp
{
    class EventLoop;
    class TcpConnection;
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    class TcpConnection :public NonCopyable,public std::enable_shared_from_this<TcpConnection>{
    public: 
        using TcpConnectionCallBack = std::function<void(const TcpConnectionPtr&)>;
    
    public:
        explicit TcpConnection(int , EventLoop*);

        void send(const string&);
        string recv();
        string recvLine();
        string show() const;

        bool isClosed() const;

        void setConnectionCallBack(const TcpConnectionCallBack&);
        void setMessageCallBack(const TcpConnectionCallBack&);
        void setCloseCallBack(const TcpConnectionCallBack&);

        void handleConnectionCallBack();
        void handleMessageCallBack();
        void handleCloseCallBack();

        void notifyLoop(const string &);
    private:
        InetAddress getLocalAddr();
        InetAddress getPeerAddr();

    private:

        Socket _clientSock;
        SocketIO _sockIO;
        InetAddress _localAddr;
        InetAddress _peerAddr;

        bool _isShutDownWrite;

        EventLoop* _loopPtr;

        TcpConnectionCallBack _onConnectionCb;
        TcpConnectionCallBack _onMessageCb;
        TcpConnectionCallBack _onCloseCb;
    };
}
