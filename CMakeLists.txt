cmake_minimum_required(VERSION 3.29.0)

project(LimitOrderBook VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


enable_testing()

add_subdirectory(googletest)

set(Headers
    Book.hpp
    Limit.hpp
    Order.hpp
    OrderPipeline.hpp
    GenerateOrders.hpp
)
set(Sources
    Book.cpp
    Limit.cpp
    Order.cpp
    OrderPipeline.cpp
    GenerateOrders.cpp
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
