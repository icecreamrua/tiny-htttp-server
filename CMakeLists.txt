cmake_minimum_required(VERSION 3.5)
project(tserver VERSION 0.1.0)

set(CXX_FLAGS "-Wall -std=c++11 -std=c++14 -g")

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")
include_directories(server/include)
file(GLOB_RECURSE SOURCE_FILES server/src/*.cpp)
add_executable(tserver ${SOURCE_FILES})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/server/bin)
