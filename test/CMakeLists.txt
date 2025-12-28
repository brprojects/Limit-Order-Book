add_executable(LimitOrderBookTests
    LimitOrderBookTests.cpp
    ExampleOrdersTests.cpp
    # add other test files
)

target_link_libraries(LimitOrderBookTests
    PRIVATE
    LimitOrderBook_lib
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(LimitOrderBookTests)