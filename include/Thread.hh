#pragma once

#include <pthread.h>
#include "NonCopyable.hh"
#include <functional>

namespace nycpp
{
    class Thread: private NonCopyable
    {
    using ThreadCallBack = std::function<void()>;

    public:
        Thread(ThreadCallBack &&);
        Thread(size_t, ThreadCallBack&&); //id , func
        ~Thread();
        void create();
        void join();
    
    private:
        // 
        static void* threadFunc(void *);
    
    private:
        size_t _workerId;
        pthread_t _tid;
        bool _isRunning;
        ThreadCallBack _cb;
    };
}