#pragma once
#include <memory>
#include <vector>
#include "Thread.hh"
#include "TaskQueue.hh"

namespace nycpp
{
    using std::unique_ptr;
    using std::vector;

    class ThreadPool
    {
        friend class WorkerThread;
    public:
        using Task = std::function<void()>;
    
    public:
        ThreadPool(size_t, size_t);
        ~ThreadPool();

    public:
        void start();
        void stop();
        void addTask(Task&&);

    private:

        void doTask();
        Task getTask();
    
    private:
        vector<std::unique_ptr<Thread>> _workers;
        size_t _workerNum;
        size_t _capacityQue;
        TaskQueue _taskQueue;
        bool _isExiting;
    };
}