#pragma once

#include <mutex>
#include <condition_variable>

namespace stdx
{

class SimpleEvent
{
public:
    SimpleEvent() : state(false) {}

    virtual void setAndNotify()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            state = true;
        }
        cv.notify_all();
    }

    explicit operator bool() const
    {
        std::unique_lock<std::mutex> lock(mutex);
        return state;
    }

    template<class Rep, class Period>
    bool wait(const std::chrono::duration<Rep, Period> &duration)
    {
        std::unique_lock<std::mutex> lock(mutex);
        return cv.wait_for(lock, duration, [this]() { return state; });
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return state; });
    }

protected:
    std::tuple<std::unique_lock<std::mutex>, bool> getLockAndState()
    {
        return { std::unique_lock<std::mutex>(mutex), state };
    }

private:
    mutable std::mutex mutex;
    std::condition_variable cv;
    bool state;
};

}