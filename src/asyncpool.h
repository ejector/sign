#ifndef ASYNCPOOL_H
#define ASYNCPOOL_H

#include <queue>
#include <thread>
#include <future>
#include <functional>

class AsyncPool final
{
public:
    AsyncPool()
        : AsyncPool(std::thread::hardware_concurrency())
    {
    }
    AsyncPool(unsigned int maximim_thread_count)
        : maximum_thead_count_(maximim_thread_count)
    {
        if (maximum_thead_count_ < 1) {
            maximum_thead_count_ = 1;
        }
    }
    void WaitIfFullAndExec(std::function<void()> func)
    {
        if (queue_.size() >= maximum_thead_count_) {
            queue_.pop();
        }
        queue_.emplace(std::async(std::launch::async, func));
    }
protected:
    std::queue<std::future<void>> queue_;
    unsigned int maximum_thead_count_ = std::thread::hardware_concurrency();
};

#endif // ASYNCPOOL_H
