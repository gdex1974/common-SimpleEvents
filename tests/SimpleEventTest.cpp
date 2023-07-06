#include <gtest/gtest.h>
#include <thread>
#include "SimpleEvent.h"

using namespace stdx;

TEST(EventTest, WaitTimeout)
{
    SimpleEvent event;
    auto start = std::chrono::steady_clock::now();
    ASSERT_FALSE(event.wait(std::chrono::milliseconds(50)));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 50);
}

TEST(EventTest, WaitNotify)
{
    SimpleEvent event;
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.setAndNotify();
    });
    auto start = std::chrono::steady_clock::now();
    ASSERT_TRUE(event.wait(std::chrono::milliseconds(100)));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 50);
    EXPECT_LT(elapsed.count(), 100);
    t.join();
}

TEST(EventTest, ConvertToBool)
{
    SimpleEvent event;
    ASSERT_FALSE(static_cast<bool>(event));
    event.setAndNotify();
    EXPECT_TRUE(static_cast<bool>(event));
}

TEST(EventTest, Wait)
{
    SimpleEvent event;
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.setAndNotify();
    });
    event.wait();
    t.join();
    EXPECT_TRUE(static_cast<bool>(event));
}

TEST(EventTest, WaitNotifyMultipleTimes)
{
    SimpleEvent event;
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.setAndNotify();
        event.setAndNotify();
    });
    event.wait();
    ASSERT_TRUE(static_cast<bool>(event));
    t.join();
}

TEST(EventTest, WaitNotifyInAnotherThread)
{
    SimpleEvent event;
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.setAndNotify();
    });
    std::thread t2([&event]() {
        EXPECT_TRUE(event.wait(std::chrono::milliseconds(100)));
    });
    t.join();
    t2.join();
}

TEST(EventTest, WaitTimeoutBeforeNotify)
{
    SimpleEvent event;
    auto start = std::chrono::steady_clock::now();
    ASSERT_FALSE(event.wait(std::chrono::milliseconds(50)));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 50);
}

TEST(EventTest, WaitTimeoutAfterNotify)
{
    SimpleEvent event;
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.setAndNotify();
    });
    auto start = std::chrono::steady_clock::now();
    ASSERT_TRUE(event.wait(std::chrono::milliseconds(100)));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 50);
    EXPECT_LT(elapsed.count(), 100);
    EXPECT_TRUE(static_cast<bool>(event));
    t.join();
}

TEST(EventTest, WaitUntilTimeout)
{
    SimpleEvent event;
    auto deadline = std::chrono::milliseconds(50);
    EXPECT_FALSE(event.wait(deadline));
}

TEST(EventTest, WaitUntilNotify)
{
    SimpleEvent event;
    auto deadline = std::chrono::milliseconds(50);
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        event.setAndNotify();
    });
    EXPECT_TRUE(event.wait(deadline));
    t.join();
}
