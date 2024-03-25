cmake_minimum_required(VERSION 3.29.0)
project(LimitOrderBook VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(LimitOrderBook BST.cpp)
target_compile_features(LimitOrderBook PUBLIC cxx_std_20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
