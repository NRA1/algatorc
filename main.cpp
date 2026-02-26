#include <cstring>
#include <filesystem>
#include <iostream>

#include "Compilation/Compilation.hpp"
#include "Compilation/CompilationInput/AlgorithmCompilationInput.hpp"
#include "Support/Configuration.hpp"
#include "Compilation/Compiler.hpp"
#include "Compilation/CompilationInput/ProjectCompilationInput.hpp"
#include "Execution/Execution.hpp"
#include "Support/FileManagement.hpp"

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

    Compiler compiler;

    ProjectLibrary project = compileAndLoad<ProjectCompilationInput, ProjectLibrary>(compiler);
    AlgorithmLibrary algorithm = compileAndLoad<AlgorithmCompilationInput, AlgorithmLibrary>(compiler);

    execute(project, algorithm);

    return 0;
}

void printHelpPage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " <project_name> <algorithm_name> <io_filename_root> <times_to_execute>"
                                              " [OPTIONS]\n";
    std::cout << std::setw(28) << std::left << "<project_name>" << "Name of project to execute\n";
    std::cout << std::setw(28) << std::left << "<algorithm_name>" << "Name of algorithm to execute\n";
    std::cout << std::setw(28) << std::left << "<io_filename_root>" << "File path root to test input file\n";
    std::cout << std::setw(28) << std::left << "<times_to_execute>" << "Number of times to run the algorithm\n";
    std::cout << std::setw(28) << std::left << "-dr <algator_data_root_path>" << "Path to \"data_root\" folder to use. If not set defaults to $ALGATOR_ROOT/data_root\n";
    std::cout << std::setw(28) << std::left << "-dl <algator_data_local_path>" << "Path to \"data_local\" folder to use. If not set defaults to $ALGATOR_ROOT/data_local\n";
    std::cout << std::setw(28) << std::left << "-c" << "Recompile even if the source files have not changed\n";
    std::cout << std::setw(28) << std::left << "-d" << "Deserialize new 'input' before each call to 'execute'\n";
}