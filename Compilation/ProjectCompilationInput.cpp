#include "ProjectCompilationInput.hpp"

#include "../Support/Error.hpp"
#include <Templates/Project.hpp>
#include <Templates/ErrorReporting.hpp>
#include <Templates/MemoryManagement.hpp>

ProjectCompilationInput::ProjectCompilationInput()
{
    try
    {
        if (!std::filesystem::exists(Configuration::temporaryDir()))
            std::filesystem::create_directories(Configuration::temporaryDir());

        if (!std::filesystem::exists(Configuration::projectBinDir()))
            std::filesystem::create_directories(Configuration::projectBinDir());
    }
    catch (std::filesystem::filesystem_error& e)
    {
        error(ErrorType::System, ErrorPhase::Preparation, "Failed to create binary directories: ") << e.what();
    }
}

std::filesystem::path ProjectCompilationInput::inputFilePath()
{
    return Configuration::temporaryDir() / "project" SOURCE_EXTENSION;
}

std::filesystem::path ProjectCompilationInput::outputFilePath()
{
    return Configuration::projectBinDir() / "project" SOURCE_EXTENSION ".so";
}

ProjectLibrary ProjectCompilationInput::loadDynamicLibrary()
{
    return CompilationInput::loadDynamicLibrary<ProjectLibrary>();
}

std::vector<std::string> ProjectCompilationInput::wrappedSymbols() const
{
    return {"malloc", "free", "calloc", "realloc"};
}

std::string ProjectCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << ErrorReportingTemplate << "\n";
    stream << "#include " << Configuration::inputSrcFilePath() << "\n";
    stream << "#include " << Configuration::outputSrcFilePath() << "\n";
    stream << "#include " << Configuration::dataConverterSrcFilePath() << "\n";
    stream << ProjectTemplate << "\n";
    stream << MemoryManagementTemplate << "\n";
    return stream.str();
}

std::vector<std::filesystem::path> ProjectCompilationInput::inputDependencies()
{
    std::vector<std::filesystem::path> dependencies;
    dependencies.push_back(Configuration::inputSrcFilePath());
    dependencies.push_back(Configuration::outputSrcFilePath());
    dependencies.push_back(Configuration::dataConverterSrcFilePath());
    return dependencies;
}