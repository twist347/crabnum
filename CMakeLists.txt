cmake_minimum_required(VERSION 3.20)
project(crabnum)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(crabnum INTERFACE)
add_library(crabnum::crabnum ALIAS crabnum)

target_include_directories(
        crabnum INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

option(BUILD_TEST "Build unit tests" ON)
option(BUILD_BENCH "Build bench" ON)

if (BUILD_TEST)
    add_subdirectory(thirdparty/googletest-1.17.0)
    enable_testing()
    add_subdirectory(test)
endif ()

if (BUILD_BENCH)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

    add_subdirectory(thirdparty/benchmark-1.9.5)
    enable_testing()
    add_subdirectory(bench)
endif ()
