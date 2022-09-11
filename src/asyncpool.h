#ifndef ASYNCPOOL_H
#define ASYNCPOOL_H

#include <iostream>
#include <queue>
#include <thread>
#include <future>
#include <functional>

#include "threadsafequeue.h"

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

        for (decltype(maximum_thead_count_) i = 0; i < maximum_thead_count_; ++i) {
            threads.emplace_back([&]() {
                while (!stop_) {
                    try {
                        auto task = tasks_.Pop();
                        if (task.valid()) {
                            task();
                        }
                    } catch (const std::exception& e) {
                        std::cout << "exception: " << e.what() << std::endl;
                    } catch (...) {
                        std::cout << "exception" << std::endl;
                    }
                }
            });
        }
    }
    ~AsyncPool()
    {
        stop_ = true;
        for (decltype(maximum_thead_count_) i = 0; i < maximum_thead_count_; ++i) {
            auto r = Async([](){});
        }
        for (decltype(maximum_thead_count_) i = 0; i < maximum_thead_count_; ++i) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
    }
    void Exec(std::function<void()> func)
    {
        if (queue_.size() >= maximum_thead_count_) {
            auto f = std::move(queue_.front());
            if (f.valid()) {
                f.get();
            }
            queue_.pop();
        }
        queue_.emplace(Async(func));
    }

    [[nodiscard]] std::future<void> Async(std::function<void()> func)
    {
        std::packaged_task<void()> task(func);
        auto result = task.get_future();
        tasks_.Push(std::move(task));
        return result;
    }

protected:
    std::queue<std::future<void>> queue_;
    unsigned int maximum_thead_count_ = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;
    bool stop_ = false;
    ThreadSafeQueue<std::packaged_task<void()>> tasks_;
};

#endif // ASYNCPOOL_H
