cmake_minimum_required(VERSION 3.25)
project(SimpleEvents)

set(CMAKE_CXX_STANDARD 17)
# Add the library
add_library(SimpleEvents STATIC
        SimpleEvent.cpp SimpleEvent.h ManualResetEvent.cpp ManualResetEvent.h)
target_compile_features(SimpleEvents PUBLIC cxx_std_17)
target_compile_options(SimpleEvents PRIVATE -Wall -Wextra -pedantic)
target_include_directories(SimpleEvents PUBLIC .)

# Add the gtest library
find_package(GTest)
if (GTEST_FOUND)
    add_subdirectory(tests)
endif()
