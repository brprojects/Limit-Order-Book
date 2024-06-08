cmake_minimum_required(VERSION 3.29.0)

project(LimitOrderBook VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


enable_testing()

add_subdirectory(googletest)

set(Headers
    ./Limit_Order_Book/Book.hpp
    ./Limit_Order_Book/Limit.hpp
    ./Limit_Order_Book/Order.hpp
    ./Process_Orders/OrderPipeline.hpp
    ./Generate_Orders/GenerateOrders.hpp
)
set(Sources
    ./Limit_Order_Book/Book.cpp
    ./Limit_Order_Book/Limit.cpp
    ./Limit_Order_Book/Order.cpp
    ./Process_Orders/OrderPipeline.cpp
    ./Generate_Orders/GenerateOrders.cpp
)

# Define the library target
add_library(${PROJECT_NAME}_lib STATIC ${Sources} ${Headers})

# Define the executable target
add_executable(${PROJECT_NAME} main.cpp)

# Link the executable with the library
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
