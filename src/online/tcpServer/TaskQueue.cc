#include "TaskQueue.hh"
#include "MutexLockGuard.hh"
#include "log4class.hh"
#include <thread>
#include <mutex>

namespace nycpp
{
    TaskQueue::TaskQueue(size_t capacity)
        :_capacity(capacity), 
         _mutex(),
         _full(_mutex),
         _empty(_mutex), 
         _isExiting(false)
    { }

    bool TaskQueue::full() const
    {
        return _taskQueue.size() == _capacity;
    }

    bool TaskQueue::empty() const
    {
        return _taskQueue.empty();   
    }

    void TaskQueue::push(Task &&task)
    {
        
        MutexLockGuard autoLocker(_mutex);

        //std::lock_guard<std::mutex>(_mutex);
        while(full())
        {
            _full.wait();
        }

        _taskQueue.push(std::move(task));

        if(!empty())
        {
            _empty.notifyAll();  // thundering herd 
        }
    }

    TaskQueue::Task TaskQueue::pop()
    {
        MutexLockGuard autoLocker(_mutex);
        while( _isExiting == false && empty() == true)
        {
            _empty.wait();
        }

        if(_isExiting == true)
        {
            return nullptr;
        }

        Task tmp = _taskQueue.front();
        _taskQueue.pop();

        if(full() == false)
        {
            _full.notifyAll();
        }

        return tmp;
    }

    /* BUG!!!! only notify once */
    void TaskQueue::wakeupAll()
    {
        _isExiting = true;
        _empty.notifyAll(); 
    }
}