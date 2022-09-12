#ifndef POOL_H
#define POOL_H

#include <functional>
#include <memory>

#include "threadsafequeue.h"

template<typename T>
class Pool final
{
public:
    template<typename... Ts>
    Pool(size_t maximum_count, Ts... args)
        : maximum_count_(maximum_count)
    {
        for (size_t i = 0; i < maximum_count_; ++i) {
           items_.emplace_back(std::forward<Ts>(args)...);
           queue_.Push(i);
        }
    }
    auto Get()
    {
        auto index = queue_.Pop();
        return std::unique_ptr<T, std::function<void(T*)>>(&items_.at(index), [this, index](T*) {
            this->queue_.Push(index);
        });
    }

protected:
    size_t maximum_count_;
    std::vector<T> items_;
    ThreadSafeQueue<size_t> queue_;
};

#endif // POOL_H
