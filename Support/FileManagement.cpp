#include "FileManagement.hpp"

#include "Configuration.hpp"
#include "Error.hpp"
#include "Guard.hpp"


std::pair<char*, unsigned int> readInputFile()
{
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

    return {buffer, file_size};
}

void writeOutputFile(const char* buffer, const unsigned int size)
{
    std::ofstream output_file;
    output_file.open(Configuration::outputFilePath(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output_file.is_open())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to open output file: ") << Configuration::outputFilePath();
    guardInternal([&]
    {
        output_file.write(buffer, size);
    });
    output_file.close();
    if (output_file.fail())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to write output file: ") << Configuration::outputFilePath();
}

void writeSuccessStatusFile(const std::vector<long int>& times)
{
    std::ofstream status_file;
    status_file.open(Configuration::statusFilePath(), std::ios::out | std::ios::trunc);
    if (!status_file.is_open())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to open status file: ") << Configuration::statusFilePath();
    status_file << "OK";

    for (const long int& elapsed : times)
        status_file << "\n" << elapsed;

    status_file.close();
    if (status_file.fail())
        error(ErrorType::System, ErrorPhase::Teardown, "Failed to write status file: ") << Configuration::statusFilePath();
}
