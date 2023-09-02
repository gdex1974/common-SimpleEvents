#pragma once

#include "SimpleEvent.h"

#include <vector>
#include <functional>

namespace stdx
{

class ManualResetEvent : public SimpleEvent
{
public:
    ManualResetEvent() = default;

    ~ManualResetEvent();

    void setAndNotify() override;

    template<typename... Args>
    static void waitForOneOf(Args &...args)
    {
        ManualResetEvent event(args...);
        event.wait();
    }

    template<typename Rep, typename Period, typename... Args>
    static bool waitForOneOf(const std::chrono::duration<Rep, Period> &duration, Args &...args)
    {
        ManualResetEvent event(args...);
        return event.wait(duration);
    }

private:
    template<typename ...Args>
    explicit ManualResetEvent(Args &... events)
    {
        (subscribeOn(events), ...);
    }

    void addSubscriber(ManualResetEvent &event)
    {
        std::unique_lock lock(subscribersMutex);
        subscribers.emplace_back(std::ref(event));
    }

    void removeSubscriber(ManualResetEvent &event)
    {
        std::unique_lock lock(subscribersMutex);
        subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(),
                                         [&event](auto &ref) { return &event == &ref.get(); }), subscribers.end());
    }

    void subscribeOn(ManualResetEvent &event);

    void unsubscribeFrom(ManualResetEvent &event);

    void removeFromSubscribed(ManualResetEvent &event);

    void removeFromSubscribers(ManualResetEvent &event);

    std::mutex subscribersMutex;
    std::mutex subscribedMutex;
    std::vector<std::reference_wrapper<ManualResetEvent>> subscribers;
    std::vector<std::reference_wrapper<ManualResetEvent>> subscribed;
};

}
