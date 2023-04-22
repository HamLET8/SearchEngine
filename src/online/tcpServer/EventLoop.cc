#include "EventLoop.hh"
#include "ErrorCheck.hh"
#include <sys/eventfd.h>
#include <unistd.h>
#include "log4class.hh"

namespace nycpp
{



EventLoop::EventLoop(Acceptor& acceptor)
    :_epFd(createEpoll()),
    _eventFd(createEvent()),
    _acceptor(acceptor),
    _isLooping(false),
    _eventList(INIT_EPOLLNUM)
{
    addEpollFd(_acceptor.serverFd());
    addEpollFd(_eventFd);
}

EventLoop::~EventLoop()
{
    while(::close(_epFd) < 0 && errno == EINTR); //ignore sys interupt
}
    
int EventLoop::createEpoll()
{
    int epFd = ::epoll_create1(0);
    ERROR_CHECK(epFd, -1, "epoll_create1");

    return epFd; 
}

int EventLoop::createEvent()
{
    int evenFd = ::eventfd(0, 0);
    ERROR_CHECK(evenFd, -1, "eventfd");
    return evenFd;
}



void EventLoop::loop()
{
    _isLooping = true;

    while(_isLooping)
    {
        waitEpoll();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::addEpollFd(int fd)
{
    logInfo("add epoll fd: %d", fd);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}

void EventLoop::delEpollFd(int fd)
{
    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_DEL, fd, nullptr);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}


/*
void EventLoop::readCounter()
{
    uint64_t val = 0;
    int ret = ::read(_eventFd, &val, sizeof(val));
    ERROR_CHECK(ret, -1, "read evenfd");
}

void EventLoop::writeCounter()
{
    uint64_t val = 1;
    int ret = ::write(_eventFd, &val, sizeof(val));  
    ERROR_CHECK(ret, -1, "write eventfd");
}
*/

void EventLoop::readCounter()
{
    uint64_t val = 0;
    int ret = ::eventfd_read(_eventFd, &val);
    ERROR_CHECK(ret, -1, "evenfd_read");
}


void EventLoop::writeCounter()
{
    uint64_t val = 1;
    int ret = ::eventfd_write(_eventFd, val);  
    ERROR_CHECK(ret, -1, "eventfd_write");
}

void EventLoop::handlePendingCbs()
{
    std::vector<PendingCallBack> pcbList;
    {
        MutexLockGuard autoLock(_mutex);
        pcbList.swap(_pendingCbs);
    }
    for(auto& worker : pcbList)
    {
        worker();   
    }
}


void EventLoop::setConnectionCallBack(EventLoopCallBack&& onConnectionCb)
{
    _onConnectionCb = onConnectionCb;
}

void EventLoop::setMessageCallBack(EventLoopCallBack&& onMessageCb)
{
    _onMessageCb = onMessageCb;
}

void EventLoop::setCloseCallBack(EventLoopCallBack&& onCloseCb)
{
    _onCloseCb = onCloseCb;
}

void EventLoop::setPendingCallBack(PendingCallBack&& onPendingCb)
{
    MutexLockGuard autoLock(_mutex);
    _pendingCbs.push_back(onPendingCb);
}


void EventLoop::handleNewConnection()
{
    int clientFd = _acceptor.accept();
    addEpollFd(clientFd);

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(clientFd, this);
    
    conn->setConnectionCallBack(_onConnectionCb); 
    conn->setMessageCallBack(_onMessageCb);
    conn->setCloseCallBack(_onCloseCb);

    logInfo("inset a connection to map");
    _connMap.insert(std::make_pair(clientFd, conn));

    if(conn)
    {
        conn->handleConnectionCallBack();
    }
    else
    {
        logError("TcpConnectionPtr is nullptr");
    }

}

void EventLoop::handleOldConnection(int clientFd)
{
    auto conn_it = _connMap.find(clientFd);

    if(conn_it != _connMap.end())
    {
        if(conn_it->second->isClosed())
        {
            conn_it->second->handleCloseCallBack();

            delEpollFd(clientFd);
            _connMap.erase(conn_it);
        }else{
            conn_it->second->handleMessageCallBack();
        }
    }
    else
    {
        logError("Can`t find client fd %d in connection map", clientFd);
    }
}

void EventLoop::waitEpoll()
{
    int readyNum = 0;

    do{
        readyNum = ::epoll_wait(_epFd, &_eventList[0], _eventList.size(), 5000);
    }while(readyNum == -1 && errno == EINTR);

    if(readyNum == -1)
    {
        ::perror("epoll_wait");
    }else if(readyNum == 0)
    {
        logError("epoll_wait time out.");
    }
    
    if(readyNum > 0)
    {
        logInfo("epoll_wait readyNum: %d", readyNum);

        if(readyNum == _eventList.size()) 
        {
            _eventList.resize(2 * readyNum);
        }

        for(int idx = 0; idx < readyNum ; ++idx)
        {
            if(_eventList[idx].data.fd == _acceptor.serverFd())
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    logInfo("new connection coming");
                    handleNewConnection();
                }    
            }else if (_eventList[idx].data.fd == _eventFd)
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    logInfo("eventfd coming");
                    readCounter();
                    handlePendingCbs();
                }   
            }else{
                if(_eventList[idx].events & EPOLLIN)
                {   
                    logInfo("old connection coming");
                    handleOldConnection(_eventList[idx].data.fd);
                }
            }
        }   
    }
}





































} // namespace server
