#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/AlgorithmCompilationInput.hpp"
#include "Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/Linker.hpp"
#include "Compilation/ProjectCompilationInput.hpp"


int main(const int argc, char* argv[])
{
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    Configuration config = Configuration::parse(argc, argv);

    Compiler compiler{};
    Linker linker{};

    ProjectCompilationInput project_input(config);
    if (project_input.compilationNeeded())
    {
        compiler.compile(project_input);
        linker.link(project_input);

        project_input.clean();
    }
    DynamicLibrary project_lib = project_input.loadDynamicLibrary();

    AlgorithmCompilationInput algorithm_input(config);
    if (algorithm_input.compilationNeeded())
    {
        compiler.compile(algorithm_input);
        linker.link(algorithm_input);

        algorithm_input.clean();
    }
    DynamicLibrary algorithm_lib = algorithm_input.loadDynamicLibrary();


    auto deserialize_input = project_lib.resolve<void*(*)(char*, unsigned int)>("deserialize_input");
    auto serialize_output = project_lib.resolve<char*(*)(void*, unsigned int*)>("serialize_output");
    auto execute = algorithm_lib.resolve<void*(*)(void*)>("execute");

    std::string str = "4 3 2 1";
    char cstr[str.length()];
    memcpy(cstr, str.c_str(), str.length());
    void* input = deserialize_input(cstr, str.length());
    void* output = execute(input);

    unsigned int n;
    char* serialized_output = serialize_output(output, &n);


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
