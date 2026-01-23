cmake_minimum_required(VERSION 4.0)
project(algatorc)

set(CMAKE_CXX_STANDARD 23)

find_package(Clang 21.1.6 REQUIRED)
find_package(LLVM 21.1.6 REQUIRED)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/Algorithm.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        DEPENDS CompilationTemplates/Algorithm.hpp
        VERBATIM
)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/Project.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        DEPENDS CompilationTemplates/Project.hpp
        VERBATIM
)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/ErrorReporting.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        DEPENDS CompilationTemplates/ErrorReporting.hpp
        VERBATIM
)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/MemoryManagement.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/MemoryManagement.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/MemoryManagement.hpp
        DEPENDS CompilationTemplates/MemoryManagement.hpp
        VERBATIM
)

add_executable(algatorc main.cpp
        Support/Configuration.cpp
        Support/Configuration.hpp
        Support/Error.cpp
        Support/Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
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
        Loading/DynamicLibrary.cpp
        Loading/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/MemoryManagement.hpp
        Loading/ProjectLibrary.cpp
        Loading/ProjectLibrary.hpp
        Loading/AlgorithmLibrary.cpp
        Loading/AlgorithmLibrary.hpp)
target_compile_definitions(algatorc PUBLIC ALGATORC)
target_link_libraries(algatorc PRIVATE clang clang-cpp)
target_link_libraries(algatorc PRIVATE LLVM)
target_include_directories(algatorc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(algatorcpp main.cpp
        Support/Configuration.cpp
        Support/Configuration.hpp
        Support/Error.cpp
        Support/Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
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
        Loading/DynamicLibrary.cpp
        Loading/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/MemoryManagement.hpp
        Loading/ProjectLibrary.cpp
        Loading/ProjectLibrary.hpp
        Loading/AlgorithmLibrary.cpp
        Loading/AlgorithmLibrary.hpp)
target_compile_definitions(algatorcpp PUBLIC ALGATORCPP)
target_link_libraries(algatorcpp PRIVATE clang clang-cpp)
target_link_libraries(algatorcpp PRIVATE LLVM)
target_include_directories(algatorcpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})