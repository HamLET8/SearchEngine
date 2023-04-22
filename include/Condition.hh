#pragma once
#include "MutexLock.hh"
#include <pthread.h>


namespace nycpp
{
    class Condition : private NonCopyable
    {
    public: 
        Condition(MutexLock&);
        ~Condition();

        void wait();
        void notify();
        void notifyAll();

    private:
        MutexLock& _mutex;
        pthread_cond_t _cond;
    }; 

}