#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/CompilationInput/AlgorithmCompilationInput.hpp"
#include "Support/Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/CompilationInput/ProjectCompilationInput.hpp"
#include "Execution/Execution.hpp"
#include "Support/FileManagement.hpp"
#include "Support/Guard.hpp"

int main(const int argc, char* argv[])
{
    bool force_recompile = true;

    Error::setPhase(ErrorPhase::Preparation);

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    Configuration::parse(argc, argv);

    Error::setPhase(ErrorPhase::Compilation);

    Compiler compiler{};

    ProjectCompilationInput project_input;
    if (force_recompile || project_input.compilationNeeded())
    {
        compiler.compile(project_input);

        project_input.clean();
    }

    AlgorithmCompilationInput algorithm_input;
    if (force_recompile || algorithm_input.compilationNeeded())
    {
        compiler.compile(algorithm_input);

        algorithm_input.clean();
    }

    Error::setPhase(ErrorPhase::Setup);

    ProjectLibrary project = project_input.loadDynamicLibrary();
    AlgorithmLibrary algorithm = algorithm_input.loadDynamicLibrary();

    auto [buffer, file_size] = readBinaryFile(Configuration::inputFilePath());
    void* input = project.deserializeInput(buffer, file_size);

    Error::setPhase(ErrorPhase::Execution);

    auto [output, times] = execute(algorithm, input);

    Error::setPhase(ErrorPhase::Teardown);

    unsigned int output_len;
    char* serialized_output = project.serializeOutput(output, &output_len);

    writeBinaryFile(Configuration::outputFilePath(), serialized_output, output_len);

    algorithm.freeAll();
    project.freeAll();

    guardInternal([&]
    {
        delete[] buffer;
    });

    writeSuccessStatusFile(times);

    return 0;
}
