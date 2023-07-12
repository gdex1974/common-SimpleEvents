#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>

namespace stdx
{

template<typename T, class Container = std::queue<T>>
class SynchronizedQueue
{
public:
    SynchronizedQueue() : stopFlag(false) {}

    template<class U=T>
    std::optional<T> push(U &&t)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (!stopFlag)
        {
            queue.push(std::forward<U>(t));
            lock.unlock();
            conditionVariable.notify_one();
            return std::nullopt;
        }
        return std::forward<U>(t);
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty() && !stopFlag)
        {
            conditionVariable.wait(lock);
        }
        if (!queue.empty())
        {
            T t = std::move(queue.front());
            queue.pop();
            return std::move(t);
        }
        return {};
    }

    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stopFlag = true;
        }
        conditionVariable.notify_all();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

private:
    Container queue;
    mutable std::mutex mutex;
    std::condition_variable conditionVariable;
    bool stopFlag;
};

}