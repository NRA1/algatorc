cmake_minimum_required(VERSION 4.0)
project(algatorc)

set(CMAKE_CXX_STANDARD 23)

add_executable(algatorc main.cpp
        configuration.cpp
        configuration.hpp
        error.cpp
        error.hpp)
target_compile_definitions(algatorc PUBLIC ALGATORC)

add_executable(algatorcpp main.cpp
        configuration.cpp
        configuration.hpp
        error.cpp
        error.hpp)
target_compile_definitions(algatorcpp PUBLIC ALGATORCPP)