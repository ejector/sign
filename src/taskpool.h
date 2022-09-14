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
    ~TaskPool() noexcept
    {
        while (!queue_.empty()) {
            WaitFrontTaskAndRemove();
        }
    }
    void Exec(std::function<void()> func)
    {
        if (queue_.size() >= count_) {
            WaitFrontTaskAndRemove();
        }
        queue_.emplace(ThreadPool::Instance().Async(func));
    }
protected:
    void WaitFrontTaskAndRemove()
    {
        auto f = std::move(queue_.front());
        if (f.valid()) {
            f.get();
        }
        queue_.pop();
    }
protected:
    std::queue<std::future<void>> queue_;
    size_t count_;
};

#endif // TASKPOOL_H
