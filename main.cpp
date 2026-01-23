#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/AlgorithmCompilationInput.hpp"
#include "Support/Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/ProjectCompilationInput.hpp"
#include "Execution/Execution.hpp"
#include "Support/FileManagement.hpp"
#include "Support/Guard.hpp"

int main(const int argc, char* argv[])
{

    bool force_recompile = true;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    Configuration::parse(argc, argv);

    Compiler compiler{};

    ProjectCompilationInput project_input;
    if (force_recompile || project_input.compilationNeeded())
    {
        compiler.compile(project_input);

        project_input.clean();
    }
    ProjectLibrary project = project_input.loadDynamicLibrary();

    AlgorithmCompilationInput algorithm_input;
    if (force_recompile || algorithm_input.compilationNeeded())
    {
        compiler.compile(algorithm_input);

        algorithm_input.clean();
    }
    AlgorithmLibrary algorithm = algorithm_input.loadDynamicLibrary();

    auto [buffer, file_size] = readInputFile();
    void* input = project.deserializeInput(buffer, file_size);

    auto [output, times] = execute(algorithm, input);

    unsigned int output_len;
    char* serialized_output = project.serializeOutput(output, &output_len);

    writeOutputFile(serialized_output, output_len);

    algorithm.freeAll();
    project.freeAll();

    guardInternal([&]
    {
        delete[] buffer;
    });

    writeSuccessStatusFile(times);

    return 0;
}
