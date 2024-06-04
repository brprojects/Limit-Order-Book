cmake_minimum_required(VERSION 3.29.0)

set(This LimitOrderBookTests)

set(Sources
    LimitOrderBookTests.cpp
    ExampleOrdersTests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    gtest_main
    LimitOrderBook_lib
)

add_test(
    NAME ${This}
    COMMAND ${This}
)