#include <gtest/gtest.h>
#include <thread>
#include "ManualResetEvent.h"

using namespace stdx;

TEST(ManualResetEventTest, Set) {
    ManualResetEvent event;

    ASSERT_FALSE(static_cast<bool>(event));
    event.setAndNotify();
    ASSERT_TRUE(static_cast<bool>(event));
}

TEST(ManualResetEventTest, WaitForOneOfWithoutTimeout) {
    ManualResetEvent event1;
    ManualResetEvent event2;
    auto start = std::chrono::steady_clock::now();
    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        event1.setAndNotify();
    });

    ManualResetEvent::waitForOneOf(event1, event2);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 100);
    ASSERT_TRUE(static_cast<bool>(event1));
    t.join();
}

TEST(ManualResetEventTest, WaitForOneOfWithTimeout) {
    // Test the waitForOneOf() method with timeout
    ManualResetEvent event1;
    ManualResetEvent event2;
    auto start = std::chrono::steady_clock::now();
    ASSERT_FALSE(ManualResetEvent::waitForOneOf(std::chrono::milliseconds(100), event1, event2));
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(elapsed.count(), 100);
    start = end;
    event1.setAndNotify();
    ASSERT_TRUE(ManualResetEvent::waitForOneOf(std::chrono::milliseconds(100), event1, event2));
    end = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(elapsed.count(), 100);
}
