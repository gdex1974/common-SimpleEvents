cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

add_executable(tests
        SimpleEventTest.cpp ManualResetEventTest.cpp SynchronizedQueueTest.cpp)
target_link_libraries(tests
        PRIVATE SimpleEvents
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(tests)