#include "CompilationInput.hpp"

#include "../Support/Error.hpp"

std::filesystem::path CompilationInput::objFilePath()
{
    return inputFilePath().replace_extension(".o");
}

void CompilationInput::writeInputFile()
{
    const std::string input = buildInputFile();
    std::ofstream file;
    file.open(inputFilePath(), std::ios::out | std::ios::trunc);
    if (!file.is_open())
        error(ErrorType::System, ErrorPhase::Compilation, "Failed to write generated input source file");
    file << input;
    file.close();
    if (file.fail())
        error(ErrorType::System, ErrorPhase::Compilation, "Failed to write generated input source file");
}

bool CompilationInput::compilationNeeded()
{
    if (!std::filesystem::exists(outputFilePath())) return true;

    const std::filesystem::file_time_type output_write_time = std::filesystem::last_write_time(outputFilePath());

    const std::vector dependencies = inputDependencies();
    std::filesystem::file_time_type max_input_write_time = std::filesystem::file_time_type::min();
    for (const std::filesystem::path& dependency : dependencies)
    {
        std::filesystem::file_time_type write_time = std::filesystem::last_write_time(dependency);
        if (write_time > max_input_write_time)
            max_input_write_time = write_time;
    }

    return max_input_write_time >= output_write_time;
}

void CompilationInput::clean()
{
    if (std::filesystem::exists(objFilePath()))
        std::filesystem::remove(objFilePath());
}

std::vector<std::string> CompilationInput::wrappedSymbols() const
{
    return {};
}
