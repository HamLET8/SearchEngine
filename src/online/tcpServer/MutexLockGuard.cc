#include "MutexLockGuard.hh"

namespace nycpp
{
    MutexLockGuard::MutexLockGuard(MutexLock& mutexLock)
    :_mutex(mutexLock)
    {
        _mutex.lock();
    }

    MutexLockGuard::~MutexLockGuard()
    {
        _mutex.unlock();
    }
}
