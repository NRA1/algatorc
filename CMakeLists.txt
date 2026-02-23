cmake_minimum_required(VERSION 4.0)
project(algatorc)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ALGAlloc)
add_subdirectory(ALGExit)

find_package(Clang 21.1 REQUIRED)
find_package(LLVM 21.1 REQUIRED)

if(WIN32)
    find_package(dlfcn-win32 REQUIRED)
    set(CMAKE_DL_LIBS dlfcn-win32::dl)
endif ()

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

add_executable(algatorc main.cpp
        Support/Configuration.cpp
        Support/Configuration.hpp
        Support/Error.cpp
        Support/Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
        Compilation/DummyConsumer.cpp
        Compilation/DummyConsumer.hpp
        Compilation/CompilationInput.cpp
        Compilation/CompilationInput.hpp
        Compilation/ProjectCompilationInput.cpp
        Compilation/ProjectCompilationInput.hpp
        Compilation/AlgorithmCompilationInput.cpp
        Compilation/AlgorithmCompilationInput.hpp
        Execution/DynamicLibrary.cpp
        Execution/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        Execution/ProjectLibrary.cpp
        Execution/ProjectLibrary.hpp
        Execution/AlgorithmLibrary.cpp
        Execution/AlgorithmLibrary.hpp
        Support/FileManagement.hpp
        Support/FileManagement.cpp
        Execution/Execution.hpp
        Execution/Execution.cpp)
target_compile_definitions(algatorc PUBLIC ALGATORC)
target_link_libraries(algatorc PRIVATE clang clang-cpp)
target_link_libraries(algatorc PRIVATE LLVM)
target_include_directories(algatorc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(algatorc PRIVATE ${CMAKE_DL_LIBS})
if (WIN32)
    target_link_libraries(algatorc PRIVATE algalloc_dummy)
    target_link_libraries(algatorc PRIVATE algexit_dummy)
else ()
    target_link_libraries(algatorc PRIVATE algalloc)
    target_link_libraries(algatorc PRIVATE algexit_dummy)
endif ()

add_executable(algatorcpp main.cpp
        Support/Configuration.cpp
        Support/Configuration.hpp
        Support/Error.cpp
        Support/Error.hpp
        Compilation/Compiler.cpp
        Compilation/Compiler.hpp
        Compilation/DummyConsumer.cpp
        Compilation/DummyConsumer.hpp
        Compilation/CompilationInput.cpp
        Compilation/CompilationInput.hpp
        Compilation/ProjectCompilationInput.cpp
        Compilation/ProjectCompilationInput.hpp
        Compilation/AlgorithmCompilationInput.cpp
        Compilation/AlgorithmCompilationInput.hpp
        Execution/DynamicLibrary.cpp
        Execution/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        Execution/ProjectLibrary.cpp
        Execution/ProjectLibrary.hpp
        Execution/AlgorithmLibrary.cpp
        Execution/AlgorithmLibrary.hpp
        Support/FileManagement.hpp
        Support/FileManagement.cpp
        Execution/Execution.hpp
        Execution/Execution.cpp)
target_compile_definitions(algatorcpp PUBLIC ALGATORCPP)
target_link_libraries(algatorcpp PRIVATE clang clang-cpp)
target_link_libraries(algatorcpp PRIVATE LLVM-21)
target_include_directories(algatorcpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(algatorcpp PRIVATE ${CMAKE_DL_LIBS})
if (WIN32)
    target_link_libraries(algatorcpp PRIVATE algalloc_dummy)
    target_link_libraries(algatorcpp PRIVATE algexit_dummy)
else ()
    target_link_libraries(algatorcpp PRIVATE algalloc)
    target_link_libraries(algatorcpp PRIVATE algexit)
endif ()
