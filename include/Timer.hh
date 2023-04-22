#pragma once

#include <functional>

namespace nycpp
{
    class Timer
    {
        using TimerCallBack = std::function<void()>;
    public:
        Timer(TimerCallBack&&, int, int);

        void start();
        void stop();
    
    private:
        int creatTimerFd();
        void setTimerFd(int, int);
        void handleRead();

    private:
        TimerCallBack _timerCb;
        int _timerFd;
        int _initTime;
        int _periodicTime;
        bool _isRunning;
    };
}