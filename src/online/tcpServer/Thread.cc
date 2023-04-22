#include "Thread.hh"
#include <iostream>
//#include <stdio.h>
#include <stdlib.h>
//#include <func.h>
#include "ThreadCheck.hh"
#include "log4class.hh"

__thread size_t __thread_id;

namespace nycpp
{

Thread::Thread(ThreadCallBack&& threadCb)
    :_tid(0), _isRunning(false), _cb(threadCb)
{ }

Thread::Thread(size_t id, ThreadCallBack&& threadCb)
    :_workerId(id), _tid(0), _isRunning(false), _cb(threadCb)
{ }


Thread::~Thread()
{
    if(_isRunning)
    {
        pthread_detach(_tid);
    }
}

void Thread::create()
{
    int ret = pthread_create(&_tid, nullptr, threadFunc, (void*)this);
    THREAD_CHECK(ret, 0, "pthread_create");    
    _isRunning = true;   
}

void Thread::join()
{
    if(_isRunning)
    {
        pthread_join(_tid, nullptr);
        _isRunning = false;
    }
}

void* Thread::threadFunc(void* threadPtr)
{
    Thread* pThread = (Thread*)threadPtr;
    __thread_id = pThread->_workerId;
    if(pThread != nullptr)
    {
        pThread->_cb();
    }
    logInfo("worker thread : %d exit.", pthread_self());

    pthread_exit(nullptr);
}

}