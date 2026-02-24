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
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/InputByteBuf.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/InputByteBuf.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/InputByteBuf.hpp
        DEPENDS CompilationTemplates/InputByteBuf.hpp
        VERBATIM
)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Templates/OutputByteBuf.hpp
        COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/read_template.rb ${CMAKE_CURRENT_SOURCE_DIR}/CompilationTemplates/OutputByteBuf.hpp ${CMAKE_CURRENT_BINARY_DIR}/Templates/OutputByteBuf.hpp
        DEPENDS CompilationTemplates/OutputByteBuf.hpp
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
        Compilation/CompilationInput/CompilationInput.cpp
        Compilation/CompilationInput/CompilationInput.hpp
        Compilation/CompilationInput/ProjectCompilationInput.cpp
        Compilation/CompilationInput/ProjectCompilationInput.hpp
        Compilation/CompilationInput/AlgorithmCompilationInput.cpp
        Compilation/CompilationInput/AlgorithmCompilationInput.hpp
        Execution/DynamicLibrary.cpp
        Execution/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/InputByteBuf.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/OutputByteBuf.hpp
        Execution/ProjectLibrary.cpp
        Execution/ProjectLibrary.hpp
        Execution/AlgorithmLibrary.cpp
        Execution/AlgorithmLibrary.hpp
        Support/FileManagement.hpp
        Support/FileManagement.cpp
        Execution/Execution.hpp
        Execution/Execution.cpp
        Compilation/TranslationUnit.cpp
        Compilation/TranslationUnit.hpp
        Compilation/CodeValidator/FunctionValidator.cpp
        Compilation/CodeValidator/FunctionValidator.hpp
        Compilation/CodeValidator/CSerializeOutputValidator.cpp
        Compilation/CodeValidator/CSerializeOutputValidator.hpp
        Compilation/TranslationInput/TranslationInput.cpp
        Compilation/TranslationInput/TranslationInput.hpp
        Compilation/TranslationInput/DataConverterTranslationInput.cpp
        Compilation/TranslationInput/DataConverterTranslationInput.hpp
        Compilation/CodeValidator/CDeserializeInputValidator.cpp
        Compilation/CodeValidator/CDeserializeInputValidator.hpp
        Compilation/CodeValidator/CppDeserializeInputValidator.cpp
        Compilation/CodeValidator/CppDeserializeInputValidator.hpp
        Compilation/CodeValidator/CppSerializeOutputValidator.cpp
        Compilation/CodeValidator/CppSerializeOutputValidator.hpp
        Compilation/CodeValidator/ExecuteValidator.cpp
        Compilation/CodeValidator/ExecuteValidator.hpp
        Compilation/TranslationInput/AlgorithmTranslationInput.cpp
        Compilation/TranslationInput/AlgorithmTranslationInput.hpp
        Compilation/CodeValidator/CodeValidator.cpp
        Compilation/CodeValidator/CodeValidator.hpp
        Compilation/CodeValidator/ClassValidator.cpp
        Compilation/CodeValidator/ClassValidator.hpp
        Compilation/CodeValidator/OutputValidator.cpp
        Compilation/CodeValidator/OutputValidator.hpp
        Compilation/CodeValidator/InputValidator.cpp
        Compilation/CodeValidator/InputValidator.hpp
        Compilation/TranslationInput/InputTranslationInput.cpp
        Compilation/TranslationInput/InputTranslationInput.hpp
        Compilation/TranslationInput/OutputTranslationInput.cpp
        Compilation/TranslationInput/OutputTranslationInput.hpp)
target_compile_definitions(algatorc PUBLIC ALGATORC)
target_link_libraries(algatorc PRIVATE libclang clang clang-cpp)
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
        Compilation/CompilationInput/CompilationInput.cpp
        Compilation/CompilationInput/CompilationInput.hpp
        Compilation/CompilationInput/ProjectCompilationInput.cpp
        Compilation/CompilationInput/ProjectCompilationInput.hpp
        Compilation/CompilationInput/AlgorithmCompilationInput.cpp
        Compilation/CompilationInput/AlgorithmCompilationInput.hpp
        Execution/DynamicLibrary.cpp
        Execution/DynamicLibrary.hpp
        Support/Guard.cpp
        Support/Guard.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Algorithm.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/Project.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/ErrorReporting.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/InputByteBuf.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Templates/OutputByteBuf.hpp
        Execution/ProjectLibrary.cpp
        Execution/ProjectLibrary.hpp
        Execution/AlgorithmLibrary.cpp
        Execution/AlgorithmLibrary.hpp
        Support/FileManagement.hpp
        Support/FileManagement.cpp
        Execution/Execution.hpp
        Execution/Execution.cpp
        Compilation/TranslationUnit.cpp
        Compilation/TranslationUnit.hpp
        Compilation/CodeValidator/FunctionValidator.cpp
        Compilation/CodeValidator/FunctionValidator.hpp
        Compilation/CodeValidator/CSerializeOutputValidator.cpp
        Compilation/CodeValidator/CSerializeOutputValidator.hpp
        Compilation/TranslationInput/TranslationInput.cpp
        Compilation/TranslationInput/TranslationInput.hpp
        Compilation/TranslationInput/DataConverterTranslationInput.cpp
        Compilation/TranslationInput/DataConverterTranslationInput.hpp
        Compilation/CodeValidator/CDeserializeInputValidator.cpp
        Compilation/CodeValidator/CDeserializeInputValidator.hpp
        Compilation/CodeValidator/CppDeserializeInputValidator.cpp
        Compilation/CodeValidator/CppDeserializeInputValidator.hpp
        Compilation/CodeValidator/CppSerializeOutputValidator.cpp
        Compilation/CodeValidator/CppSerializeOutputValidator.hpp
        Compilation/CodeValidator/ExecuteValidator.cpp
        Compilation/CodeValidator/ExecuteValidator.hpp
        Compilation/TranslationInput/AlgorithmTranslationInput.cpp
        Compilation/TranslationInput/AlgorithmTranslationInput.hpp
        Compilation/CodeValidator/CodeValidator.cpp
        Compilation/CodeValidator/CodeValidator.hpp
        Compilation/CodeValidator/ClassValidator.cpp
        Compilation/CodeValidator/ClassValidator.hpp
        Compilation/CodeValidator/OutputValidator.cpp
        Compilation/CodeValidator/OutputValidator.hpp
        Compilation/CodeValidator/InputValidator.cpp
        Compilation/CodeValidator/InputValidator.hpp
        Compilation/TranslationInput/InputTranslationInput.cpp
        Compilation/TranslationInput/InputTranslationInput.hpp
        Compilation/TranslationInput/OutputTranslationInput.cpp
        Compilation/TranslationInput/OutputTranslationInput.hpp)
target_compile_definitions(algatorcpp PUBLIC ALGATORCPP)
target_link_libraries(algatorcpp PRIVATE libclang clang clang-cpp)
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
