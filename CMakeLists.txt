cmake_minimum_required(VERSION 3.14)
project(LZW_CPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(LZW STATIC src/LZW.cpp src/LZW.h)
