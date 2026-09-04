cmake_minimum_required(VERSION 3.20)
project(ThreadPool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ThreadPool STATIC src/ThreadPool.cpp)
target_include_directories(ThreadPool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ThreadPool_test tests/ThreadPool_test.cpp)
target_link_libraries(ThreadPool_test PRIVATE ThreadPool)