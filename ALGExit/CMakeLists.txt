cmake_minimum_required(VERSION 4.0)
project(algatorc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    find_package(dlfcn-win32 REQUIRED)
    set(CMAKE_DL_LIBS dlfcn-win32::dl)
endif ()

add_library(algexit SHARED Algexit.cpp Algexit.hpp)
target_link_libraries(algexit ${CMAKE_DL_LIBS})

add_library(algexit_dummy SHARED AlgexitDummy.cpp Algexit.hpp)
