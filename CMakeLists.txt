cmake_minimum_required(VERSION 3.22)


set(PROJECT_NAME Webproc)
project(${PROJECT_NAME} VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

message("-- Using Compiler:${CMAKE_CXX_COMPILER}")
