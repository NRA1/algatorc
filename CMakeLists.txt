cmake_minimum_required(VERSION 4.0)
project(algatorc)

set(CMAKE_CXX_STANDARD 23)

find_package(Clang 21.1.6 REQUIRED)
find_package(LLVM 21.1.6 REQUIRED)
find_package(LLD 21.1.6 REQUIRED)

add_executable(algatorc main.cpp
        Configuration.cpp
        Configuration.hpp
        Error.cpp
        Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
        Compilation/Linker.cpp
        Compilation/Linker.hpp
        Compilation/DummyConsumer.cpp
        Compilation/DummyConsumer.hpp
        Compilation/CompilationDiagnosticsHandler.cpp
        Compilation/CompilationDiagnosticsHandler.hpp
        Compilation/CompilationInput.cpp
        Compilation/CompilationInput.hpp
        Compilation/ProjectCompilationInput.cpp
        Compilation/ProjectCompilationInput.hpp
        Compilation/AlgorithmCompilationInput.cpp
        Compilation/AlgorithmCompilationInput.hpp
        DynamicLibrary.cpp
        DynamicLibrary.hpp)
target_compile_definitions(algatorc PUBLIC ALGATORC)
target_link_libraries(algatorc PRIVATE clang clang-cpp)
target_link_libraries(algatorc PRIVATE LLVM)
target_link_libraries(algatorc PRIVATE lldCommon lldELF)

add_executable(algatorcpp main.cpp
        Configuration.cpp
        Configuration.hpp
        Error.cpp
        Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
        Compilation/Linker.cpp
        Compilation/Linker.hpp
        Compilation/DummyConsumer.cpp
        Compilation/DummyConsumer.hpp
        Compilation/CompilationDiagnosticsHandler.cpp
        Compilation/CompilationDiagnosticsHandler.hpp
        Compilation/CompilationInput.cpp
        Compilation/CompilationInput.hpp
        Compilation/ProjectCompilationInput.cpp
        Compilation/ProjectCompilationInput.hpp
        Compilation/AlgorithmCompilationInput.cpp
        Compilation/AlgorithmCompilationInput.hpp
        DynamicLibrary.cpp
        DynamicLibrary.hpp)
target_compile_definitions(algatorcpp PUBLIC ALGATORCPP)
target_link_libraries(algatorcpp PRIVATE clang clang-cpp)
target_link_libraries(algatorcpp PRIVATE LLVM)
target_link_libraries(algatorcpp PRIVATE lldCommon lldELF)