cmake_minimum_required(VERSION 3.29.0)

project(LimitOrderBook VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(googletest)

set(Headers
    Book.hpp
    Limit.hpp
    Order.hpp
)
set(Sources
    Book.cpp
    Limit.cpp
    Order.cpp
)

# add_executable(${PROJECT_NAME} main.cpp ${Sources} ${Headers})
add_library(${PROJECT_NAME} STATIC ${Sources} ${Headers})

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
