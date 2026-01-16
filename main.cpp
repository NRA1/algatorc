#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/AlgorithmCompilationInput.hpp"
#include "Support/Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/Linker.hpp"
#include "Compilation/ProjectCompilationInput.hpp"
#include <llvm/Support/CrashRecoveryContext.h>

#include "Compilation/TestCi.hpp"
#include "Support/Guard.hpp"

class input;
class output;
extern "C" output* __execute(input* input);

int main(const int argc, char* argv[])
{
    // auto a = __execute(nullptr);

    bool force_recompile = true;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    Configuration config = Configuration::parse(argc, argv);

    Compiler compiler{};
    Linker linker{};

    TestCI ci(config);
    compiler.compile(ci);
    linker.link(ci);
    DynamicLibrary ci_lib = ci.loadDynamicLibrary();
    auto test = ci_lib.resolve<void(*)()>("test");
    guard<void>("__deserialize_input", nullptr, nullptr, [&]()
    {
        // return deserialize_input(cstr, str.length());
        test();
    });


    ProjectCompilationInput project_input(config);
    if (force_recompile || project_input.compilationNeeded())
    {
        compiler.compile(project_input);
        linker.link(project_input);

        project_input.clean();
    }
    DynamicLibrary project_lib = project_input.loadDynamicLibrary();

    AlgorithmCompilationInput algorithm_input(config);
    if (force_recompile || algorithm_input.compilationNeeded())
    {
        compiler.compile(algorithm_input);
        linker.link(algorithm_input);

        algorithm_input.clean();
    }
    DynamicLibrary algorithm_lib = algorithm_input.loadDynamicLibrary();


    auto deserialize_input = project_lib.resolve<void*(*)(char*, unsigned int)>("__deserialize_input");
    auto serialize_output = project_lib.resolve<char*(*)(void*, unsigned int*)>("__serialize_output");
    auto execute = algorithm_lib.resolve<void*(*)(void*)>("__execute");

    auto project_error = project_lib.resolve<char*(*)()>("__error");
    auto project_clear_error = project_lib.resolve<void(*)()>("__clear_error");
    auto algorithm_error = algorithm_lib.resolve<char*(*)()>("__error");
    auto algorithm_clear_error = algorithm_lib.resolve<void(*)()>("__clear_error");

    std::string str = "4 3 2 1";
    char cstr[str.length()];
    memcpy(cstr, str.c_str(), str.length());

    void* input = guard<void*>("__deserialize_input", project_error, project_clear_error, [&]()
    {
        return deserialize_input(cstr, str.length());
    });

    void* output = guard<void*>("algorithm", algorithm_error, algorithm_clear_error, [&]()
    {
        return execute(input);
    });

    unsigned int n;
    char* serialized_output = guard<char*>("__serialize_output", project_error, project_clear_error, [&]()
    {
        return serialize_output(output, &n);
    });


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
