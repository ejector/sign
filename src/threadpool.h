#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <queue>
#include <thread>
#include <future>
#include <functional>

#include "threadsafequeue.h"

class ThreadPool final
{
public:
    ThreadPool()
        : ThreadPool(std::thread::hardware_concurrency())
    {
    }
    ThreadPool(unsigned int maximim_thread_count)
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
                        std::cout << "Fatal error: " << e.what() << ". Terminate." << std::endl;
                        throw;
                    }
                }
            });
        }
    }
    ~ThreadPool() noexcept
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

    static ThreadPool& Instance()
    {
        static ThreadPool thread_pool;
        return thread_pool;
    }

    unsigned int MaximumThreadCount() const
    {
        return maximum_thead_count_;
    }

    [[nodiscard]] std::future<void> Async(std::function<void()> func)
    {
        std::packaged_task<void()> task(func);
        auto result = task.get_future();
        tasks_.Push(std::move(task));
        return result;
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

protected:
    unsigned int maximum_thead_count_ = std::thread::hardware_concurrency();
    bool stop_ = false;
    std::vector<std::thread> threads;
    ThreadSafeQueue<std::packaged_task<void()>> tasks_;
};

#endif // THREADPOOL_H
