#pragma once
#include "Acceptor.hh"
#include "TcpConnection.hh"
#include "MutexLockGuard.hh"

#include <sys/epoll.h>
#include <vector>
#include <map>

namespace nycpp
{
    class EventLoop
    {
        friend void TcpConnection::notifyLoop(const string&);
    public:
        using EventLoopCallBack = TcpConnection::TcpConnectionCallBack;
    
    private:
        using EventList = std::vector<struct epoll_event>;
        using ConnectionMap = std::map<int, TcpConnectionPtr>;
        using PendingCallBack = std::function<void()>;    


    public:
        EventLoop(Acceptor&);
        ~EventLoop();

        void loop();
        void unloop();

        void setConnectionCallBack(EventLoopCallBack &&);
        void setMessageCallBack(EventLoopCallBack &&);
        void setCloseCallBack(EventLoopCallBack &&);
        void setPendingCallBack(PendingCallBack &&);

    private:
    int createEpoll();
    void addEpollFd(int);
    void delEpollFd(int);

    int createEvent();
    void readCounter();
    void writeCounter();

    void waitEpoll();
    void handleNewConnection();
    void handleOldConnection(int);
    void handlePendingCbs();

    private:
        const size_t INIT_EPOLLNUM = 1024;
        int _epFd;
        int _eventFd;

        Acceptor& _acceptor;
        bool _isLooping;
        EventList _eventList;
        ConnectionMap _connMap;

        EventLoopCallBack _onConnectionCb;
        EventLoopCallBack _onMessageCb;
        EventLoopCallBack _onCloseCb;

        MutexLock _mutex;
        std::vector<PendingCallBack> _pendingCbs;
    };

}