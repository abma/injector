 cmake_minimum_required(VERSION 2.6.4)
 #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -static")
 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static")
 project(MyProject)
 add_executable(injector inject.cpp)
 add_executable(test test.cpp)
 add_library(hook SHARED hook.cpp)
 target_link_libraries(hook -lwtsapi32)
 