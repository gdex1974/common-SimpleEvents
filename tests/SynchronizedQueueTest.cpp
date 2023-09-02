#include "SynchronizedQueue.h"
#include <gtest/gtest.h>
#include <thread>

TEST(SynchronizedQueueTest, BasicFunctionality) {
    stdx::SynchronizedQueue<int> queue;
    EXPECT_EQ(queue.size(), 0);
    auto result = queue.push(10);
    ASSERT_FALSE(result);
    EXPECT_EQ(queue.size(), 1);
    EXPECT_FALSE(queue.push(2));
    EXPECT_EQ(queue.size(), 2);
    result = queue.pop();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 10);
    EXPECT_EQ(queue.size(), 1);
    queue.stop();
    EXPECT_FALSE(queue.empty());
    EXPECT_TRUE(queue.push(1));
    result = queue.pop();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 2);
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.empty());
}

TEST(SynchronizedQueueTest, ConsumerThread)
{
    stdx::SynchronizedQueue<int> myQueue;

    int sum = 0;

    // Start a thread to consume items from the queue
    std::thread consumer([&] {
        int previous = -1;
        while (true) {
            auto data = myQueue.pop();
            if (!data) {
                break;
            }
            EXPECT_LT(previous, *data);
            previous = *data;
            sum += previous;
        }
    });

    // Add some items to the queue
    for (int i = 0; i <= 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ASSERT_FALSE(myQueue.push(i));
    }

    // Stop the queue
    myQueue.stop();

    // Wait for the consumer thread to finish
    consumer.join();
    EXPECT_EQ(sum, 55);
}
