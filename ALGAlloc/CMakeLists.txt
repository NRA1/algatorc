cmake_minimum_required(VERSION 4.0)
project(algatorc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_library(algalloc SHARED Algalloc.cpp Algalloc.hpp)
add_library(algalloc_dummy SHARED Algalloc.hpp AlgallocDummy.cpp)
