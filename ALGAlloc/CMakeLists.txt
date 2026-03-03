cmake_minimum_required(VERSION 3.31)
project(algatorc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    find_package(dlfcn-win32 REQUIRED)
    set(CMAKE_DL_LIBS dlfcn-win32::dl)
endif ()

add_library(algalloc SHARED Algalloc.cpp Algalloc.hpp)
target_link_libraries(algalloc ${CMAKE_DL_LIBS})

add_library(algalloc_dummy SHARED Algalloc.hpp AlgallocDummy.cpp)