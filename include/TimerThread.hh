#pragma once

#include "Thread.hh"
#include "Timer.hh"

namespace nycpp
{
    // TimerThread is a thread that runs a timer
    class TimerThread
    {
        using TimerCallBack = std::function<void()>;

    public:
        TimerThread(TimerCallBack&&, int, int);
        ~TimerThread();

        void start();
        void stop();
    private:
        Thread _thread;
        Timer _timer;
    };

}