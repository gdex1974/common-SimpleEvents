#include "ManualResetEvent.h"

namespace stdx
{

ManualResetEvent::~ManualResetEvent()
{
    {
        std::unique_lock lock(subscribersMutex);
        for (auto event: subscribers)
        {
            event.get().removeFromSubscribed(*this);
        }
        subscribers.clear();
    }
    {
        std::unique_lock lock(subscribedMutex);
        for (auto event: subscribed)
        {
            event.get().removeFromSubscribers(*this);
        }
        subscribed.clear();
    }
}

void ManualResetEvent::removeFromSubscribed(ManualResetEvent &event)
{
    std::unique_lock lock(subscribedMutex);
    subscribed.erase(
            std::remove_if(subscribed.begin(), subscribed.end(), [&event](auto &ref) { return &event == &ref.get(); }),
            subscribed.end());
}

void ManualResetEvent::removeFromSubscribers(ManualResetEvent &event)
{
    std::unique_lock lock(subscribersMutex);
    subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(),
                                     [&event](auto &ref) { return &event == &ref.get(); }), subscribers.end());
}

void ManualResetEvent::setAndNotify()
{
    SimpleEvent::setAndNotify();
    std::lock_guard lock(subscribersMutex);
    for (auto &subscriber: subscribers)
    {
        subscriber.get().setAndNotify();
    }
}

void ManualResetEvent::unsubscribeFrom(ManualResetEvent &event)
{
    {
        std::unique_lock lock(subscribedMutex);
        subscribed.erase(std::remove_if(subscribed.begin(), subscribed.end(),
                                        [&event](auto &ref) { return &event == &ref.get(); }), subscribed.end());
    }
    event.removeSubscriber(*this);
}

void ManualResetEvent::subscribeOn(ManualResetEvent &event)
{
    {
        std::unique_lock lock(subscribedMutex);
        subscribed.emplace_back(std::ref(event));
    }
    bool eventState = [this, &event]()
    {
        auto [lock, state] = event.getLockAndState();
        event.addSubscriber(*this);
        return state;
    }();
    if (eventState)
    {
        setAndNotify();
    }
}

}
