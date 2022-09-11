#ifndef TASKPOOL_H
#define TASKPOOL_H

#include <functional>
#include <future>
#include <queue>

#include "threadpool.h"

class TaskPool final
{
public:
    TaskPool()
        : TaskPool(ThreadPool::Instance().MaximumThreadCount())
    {
    }
    TaskPool(size_t count)
        : count_(count)
    {
    }
    void WaitIfFullAndExec(std::function<void()> func)
    {
        if (queue_.size() >= count_) {
            auto f = std::move(queue_.front());
            if (f.valid()) {
                f.get();
            }
            queue_.pop();
        }
        queue_.emplace(ThreadPool::Instance().Async(func));
    }
protected:
    std::queue<std::future<void>> queue_;
    size_t count_;
};

#endif // TASKPOOL_H
