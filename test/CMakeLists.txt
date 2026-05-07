cmake_minimum_required(VERSION 3.29.0)

# 1. Find GTest
find_package(GTest REQUIRED)

# 2. Define the executable and its source files
set(This LimitOrderBookTests)
set(Sources 
    LimitOrderBookTests.cpp 
    ExampleOrdersTests.cpp
)

add_executable(${This} ${Sources})

# 3. Link libraries (Must come AFTER add_executable)
# Use GTest::gtest_main (the namespaced version) for better compatibility
target_link_libraries(${This} 
    PRIVATE 
    GTest::gtest_main 
    LimitOrderBook_lib
)

# 4. Enable testing and add the test
enable_testing()
add_test(
    NAME ${This} 
    COMMAND ${This}
)

