#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/AlgorithmCompilationInput.hpp"
#include "Support/Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/ProjectCompilationInput.hpp"

#include "Support/Guard.hpp"

class input;
class output;
extern "C" output* __execute(input* input);

int main(const int argc, char* argv[])
{

    bool force_recompile = true;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    Configuration config = Configuration::parse(argc, argv);

    Compiler compiler{};

    ProjectCompilationInput project_input(config);
    if (force_recompile || project_input.compilationNeeded())
    {
        compiler.compile(project_input);

        project_input.clean();
    }
    ProjectLibrary project = project_input.loadDynamicLibrary();

    AlgorithmCompilationInput algorithm_input(config);
    if (force_recompile || algorithm_input.compilationNeeded())
    {
        compiler.compile(algorithm_input);

        algorithm_input.clean();
    }
    AlgorithmLibrary algorithm = algorithm_input.loadDynamicLibrary();



    std::string str = "4 3 2 1";
    char cstr[str.length()];
    memcpy(cstr, str.c_str(), str.length());

    void* input = project.deserializeInput(cstr, str.length());

    void* output = algorithm.execute(input);

    unsigned int n;
    char* serialized_output = project.serializeOutput(output, &n);


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
