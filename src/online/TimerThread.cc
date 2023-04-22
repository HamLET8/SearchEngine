#include "TimerThread.hh"
#include "log4class.hh"
#include "Thread.hh"
#include "ThreadPool.hh"


namespace nycpp
{

    TimerThread::TimerThread(TimerCallBack && _cb, int init, int periodic)
        :_thread(std::bind(&Timer::start, &_timer)),
        _timer(std::move(_cb), init, periodic)
        { }

    TimerThread::~TimerThread()
    { }

    void TimerThread::start()
    {
        _thread.create();
    }

    void TimerThread::stop()
    {
        _timer.stop();
        _thread.join();
    }

}