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

void printHelpPage(const char* program_name);

int main(const int argc, char* argv[])
{
    Error::setPhase(ErrorPhase::Preparation);

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        printHelpPage(argv[0]);
        return 0;
    }

    Configuration::parse(argc, argv);

    Error::setPhase(ErrorPhase::Compilation);

    Compiler compiler{};

    ProjectCompilationInput project_input;
    if (project_input.compilationNeeded())
    {
        compiler.compile(project_input);

        project_input.clean();
    }

    AlgorithmCompilationInput algorithm_input;
    if (algorithm_input.compilationNeeded())
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

void printHelpPage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " <project_name> <algorithm_name> <io_filename_root> <times_to_execute>"
                                              " [-dr <algator_data_root_path>] [-dl <algator_data_local_path>]\n";
    std::cout << std::setw(28) << std::left << "<project_name>" << "Name of project to execute\n";
    std::cout << std::setw(28) << std::left << "<algorithm_name>" << "Name of algorithm to execute\n";
    std::cout << std::setw(28) << std::left << "<io_filename_root>" << "File path root to test input file\n";
    std::cout << std::setw(28) << std::left << "<times_to_execute>" << "Number of times to run the algorithm\n";
    std::cout << std::setw(28) << std::left << "<algator_data_root_path>" << "Path to \"data_root\" folder to use. If not set defaults to $ALGATOR_ROOT/data_root\n";
    std::cout << std::setw(28) << std::left << "<algator_data_local_path>" << "Path to \"data_local\" folder to use. If not set defaults to $ALGATOR_ROOT/data_local\n";
}