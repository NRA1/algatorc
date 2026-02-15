#include "FileManagement.hpp"

#include "Configuration.hpp"
#include "Error.hpp"
#include "Guard.hpp"


std::string readTextFile(const std::filesystem::path& path)
{
    std::string line;
    std::string buffer;

    std::ifstream input_file;
    input_file.open(path, std::ios::in);
    if (!input_file.is_open())
        error(ErrorType::System, "Failed to open file: ") << path;

    while (getline(input_file, line))
    {
        buffer += line;
        buffer += "\n";
    }

    input_file.close();
    if (input_file.bad())
        error(ErrorType::System, "Failed to read file: ") << path;
    return buffer;
}

std::pair<char*, unsigned int> readBinaryFile(const std::filesystem::path& path)
{
    std::ifstream input_file;
    input_file.open(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!input_file.is_open())
        error(ErrorType::System, "Failed to open file: ") << path;
    std::streampos file_size = input_file.tellg();
    char* buffer = new char[file_size];
    input_file.seekg(0, std::ios::beg);
    input_file.read(buffer, file_size);
    input_file.close();
    if (input_file.fail())
        error(ErrorType::System, "Failed to read file: ") << path;

    return {buffer, file_size};
}

void writeBinaryFile(const std::filesystem::path& path, const char* buffer, const unsigned int size)
{
    std::ofstream output_file;
    output_file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output_file.is_open())
        error(ErrorType::System, "Failed to open file: ") << path;
    guardInternal([&]
    {
        output_file.write(buffer, size);
    });
    output_file.close();
    if (output_file.fail())
        error(ErrorType::System, "Failed to write file: ") << path;
}

void writeSuccessStatusFile(const std::vector<long int>& times)
{
    std::ofstream status_file;
    status_file.open(Configuration::statusFilePath(), std::ios::out | std::ios::trunc);
    if (!status_file.is_open())
        error(ErrorType::System, "Failed to open status file: ") << Configuration::statusFilePath();
    status_file << "OK";

    for (const long int& elapsed : times)
        status_file << "\n" << elapsed;

    status_file.close();
    if (status_file.fail())
        error(ErrorType::System, "Failed to write status file: ") << Configuration::statusFilePath();
}
