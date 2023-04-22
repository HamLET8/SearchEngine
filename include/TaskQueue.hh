#pragma once

#include "Condition.hh"
#include "MutexLock.hh"
#include <queue>
#include <functional>

namespace nycpp
{
    class TaskQueue
    {
        using Task = std::function<void()>;
    public:
        TaskQueue(size_t);
        
        bool full() const;
        bool empty() const;
        
        void push(Task&&);
        Task pop();

        void wakeupAll(); //for exit , wake up all thread which is waiting for 
    private:
        std::queue<Task> _taskQueue;
        size_t _capacity;
        MutexLock _mutex;
        Condition _full;
        Condition _empty;
        bool _isExiting;
    };

}
