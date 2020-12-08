cmake_minimum_required(VERSION 3.17)
project(Bootstrap)

set(CMAKE_CXX_STANDARD 20)

add_library(Bootstrap SHARED dllmain.cpp)