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


    std::ifstream input_file;
    input_file.open(Configuration::inputFilePath(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!input_file.is_open())
        error(ErrorType::System, ErrorPhase::Setup, "Failed to open input file: ") << Configuration::inputFilePath();
    std::streampos file_size = input_file.tellg();
    char* buffer = new char[file_size];
    input_file.seekg(0, std::ios::beg);
    input_file.read(buffer, file_size);
    input_file.close();
    if (input_file.fail())
        error(ErrorType::System, ErrorPhase::Setup, "Failed to read input file: ") << Configuration::inputFilePath();

    void* input = project.deserializeInput(buffer, file_size);

    void* output;
    for (int i = 0; i < 1000; i++)
        output = algorithm.execute(input);

    unsigned int n;
    char* serialized_output = project.serializeOutput(output, &n);

    std::ofstream output_file;
    output_file.open(Configuration::outputFilePath(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output_file.is_open())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to open output file: ") << Configuration::outputFilePath();
    guardInternal([&]
    {
        output_file.write(serialized_output, n);
    });
    output_file.close();
    if (output_file.fail())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to write output file: ") << Configuration::outputFilePath();

    algorithm.freeAll();
    project.freeAll();

    guardInternal([&]
    {
        delete[] buffer;
    });

    std::ofstream status_file;
    status_file.open(Configuration::statusFilePath(), std::ios::out | std::ios::trunc);
    if (!status_file.is_open())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to open status file: ") << Configuration::statusFilePath();
    status_file << "OK" << std::endl;
    status_file.close();
    if (status_file.fail())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to write status file: ") << Configuration::statusFilePath();

    return 0;
}
