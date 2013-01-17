 cmake_minimum_required(VERSION 2.6.4)
 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
 project(MyProject)
 add_executable(inject inject.cpp)
 add_executable(test test.cpp)
 add_library(hook SHARED hook.cpp)
 