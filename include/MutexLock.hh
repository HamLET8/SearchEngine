#pragma once
#include <pthread.h>
#include "NonCopyable.hh"


namespace nycpp
{
    class MutexLock : private NonCopyable
    {
    public:
        MutexLock();
        ~MutexLock();

        void lock();
        void unlock();
        pthread_mutex_t* getLock();
    private:
        pthread_mutex_t _mutex;
    };
}