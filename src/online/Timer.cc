#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <poll.h>


#include "Timer.hh"
#include "ErrorCheck.hh"
#include "log4class.hh"

namespace nycpp
{
    Timer::Timer(TimerCallBack && _cb, int init, int periodic)
        :_timerCb(std::move(_cb)),
         _timerFd(),
         _initTime(init),
         _periodicTime(periodic),
         _isRunning(false)
    { }

    void Timer::start()
    {
        _timerFd = creatTimerFd();
        setTimerFd(_initTime, _periodicTime);

        struct pollfd pfd;
        pfd.events = POLLIN;

        pfd.fd = _timerFd;
        _isRunning = true;
    
        while(_isRunning)
        {
            int nready = -1;

            do
            {
                nready = ::poll(&pfd, 1, -1);
            } while (nready == -1 && errno == EINTR);

            if(nready == -1)
            {
                logError("poll = -1");
                return;
            }
            else if(0 == nready)
            {
                logError("poll = 0 , timeout");
                return;
            }
            else // nready > 0
            {
                if(pfd.revents & POLLIN)
                {
                    handleRead();
                    if(_timerCb)
                    {
                        _timerCb();
                    }
                }
            }
        }
    }

    void Timer::stop()
    {
        if(_isRunning)
        {
            _isRunning = false;
            setTimerFd(0, 0);
        }
    }

    void Timer::handleRead()
    {
        uint64_t howmany;
        // read will clear the timerfd
        int ret = ::read(_timerFd, &howmany, sizeof(howmany));
    
        if(ret != sizeof(howmany))
        {
            logError("read error");
        }
    }
    
    
    int Timer::creatTimerFd()
    {
        int fd = timerfd_create(CLOCK_REALTIME, 0);
        
        if(fd < 0)
        {
            logError("timefd_creat");
            exit(EXIT_FAILURE);
        }

        return fd;
    }

    void Timer::setTimerFd(int init, int periodic)
    {
    
        struct itimerspec value; 
        value.it_value.tv_sec = init;
        value.it_value.tv_nsec = 0;
        
        // set periodic time 
        value.it_interval.tv_sec = periodic;
        value.it_interval.tv_nsec = 0;
        int ret = ::timerfd_settime(_timerFd, 0, &value, nullptr);

        if(ret < 0)
        {
            logError("timefd_settime failed");
        }
    }




}


