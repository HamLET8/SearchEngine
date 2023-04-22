#include "ThreadPool.hh"
#include "log4class.hh"
#include <memory>

namespace nycpp
{
    ThreadPool::ThreadPool(size_t workerNum,size_t capacityQue)
        :_workerNum(workerNum),
         _capacityQue(capacityQue),
         _taskQueue(_capacityQue),
         _isExiting(false)
        { 
            _workers.reserve(workerNum); 
        }
    
    ThreadPool::~ThreadPool()
    {
        stop();
    }

    void ThreadPool::addTask(Task&& task)
    {
        if(task)
        {
            _taskQueue.push(std::move(task));
        }
    }

    ThreadPool::Task ThreadPool::getTask()
    {
        return _taskQueue.pop();   
    }

    void ThreadPool::doTask()
    {
        while(!_isExiting)
        {
            Task task = getTask();
            if(task)
            {
                task();
            }
        }
    }

    void ThreadPool::start()
    {
        for(size_t idx = 0; idx != _workerNum; ++idx)
        {
            unique_ptr<Thread> worker(new Thread(idx, std::bind(&ThreadPool::doTask, this)));
            
            _workers.emplace_back(std::move(worker));
            
            _workers[idx]->create();
        }
    }


    void ThreadPool::stop()
    {
        if(_isExiting == false)
        {
            while(_taskQueue.empty() == false)
            {
                _taskQueue.wakeupAll();
            }

            _isExiting = true;

            for(auto& worker : _workers)
            {
                worker->join();
            }            
        }
    }    
}