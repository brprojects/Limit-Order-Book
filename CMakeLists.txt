cmake_minimum_required(VERSION 3.20)
project(LimitOrderBook VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Source files
set(SOURCES
    Limit_Order_Book/Book.cpp
    Limit_Order_Book/Limit.cpp
    Limit_Order_Book/Order.cpp
    Process_Orders/OrderPipeline.cpp
    Generate_Orders/GenerateOrders.cpp
    FIX_Protocol/FIXMessage.cpp
    FIX_Protocol/FIXEngine.cpp
)

# Create static library
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

# Include directories (critical for finding headers)
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Limit_Order_Book
    ${CMAKE_CURRENT_SOURCE_DIR}/Process_Orders
    ${CMAKE_CURRENT_SOURCE_DIR}/Generate_Orders
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX_Protocol
)

# Main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# FIX Demo executable (if you use it)
add_executable(FIXDemo FIX_Protocol/FIXDemo.cpp)
target_link_libraries(FIXDemo PRIVATE ${PROJECT_NAME}_lib)

# Packaging (optional)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)