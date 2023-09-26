#pragma once
#include "MutexLock.hh"

namespace nycpp
{
    class MutexLockGuard
    {
    public:
        MutexLockGuard(MutexLock&);
        ~MutexLockGuard();
    private:
        MutexLock &_mutex;
    };
}