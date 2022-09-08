#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>

template <typename T>
class ThreadSafeQueue
{
public:
    void Push(T value)
    {
        std::unique_lock lock(mutex_);
        queue_.emplace(std::move(value));
        lock.unlock();
        condition_.notify_one();
    }
    T Pop()
    {
        std::unique_lock lock(mutex_);
        condition_.wait(lock, [&](){
            return !queue_.empty();
        });
        if (queue_.empty()) {
            throw std::logic_error("Queue is empty, cannot pop item!");
        }
        T temp = std::move(queue_.back());
        queue_.pop();

        return temp;
    }
protected:
    std::condition_variable condition_;
    std::queue<T> queue_;
    std::mutex mutex_;
};

#endif // THREADSAFEQUEUE_H
