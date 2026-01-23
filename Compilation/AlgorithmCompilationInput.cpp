#include "AlgorithmCompilationInput.hpp"

#include "../Support/Error.hpp"
#include <Templates/Algorithm.hpp>
#include <Templates/ErrorReporting.hpp>
#include <Templates/MemoryManagement.hpp>

AlgorithmCompilationInput::AlgorithmCompilationInput()
{
    try
    {
        if (!std::filesystem::exists(Configuration::temporaryDir()))
            std::filesystem::create_directories(Configuration::temporaryDir());

        if (!std::filesystem::exists(Configuration::algorithmBinDir()))
            std::filesystem::create_directories(Configuration::algorithmBinDir());
    }
    catch (std::filesystem::filesystem_error& e)
    {
        error(ErrorType::System, ErrorPhase::Preparation, "Failed to create binary directories: ") << e.what();
    }
}

std::filesystem::path AlgorithmCompilationInput::inputFilePath()
{
    return Configuration::temporaryDir() / "algorithm" SOURCE_EXTENSION;
}

std::filesystem::path AlgorithmCompilationInput::outputFilePath()
{
    return Configuration::algorithmBinDir() / "algorithm" SOURCE_EXTENSION ".so";
}

AlgorithmLibrary AlgorithmCompilationInput::loadDynamicLibrary()
{
    return CompilationInput::loadDynamicLibrary<AlgorithmLibrary>();
}

std::vector<std::string> AlgorithmCompilationInput::wrappedSymbols() const
{
    return {"malloc", "free", "calloc", "realloc"};
}

std::string AlgorithmCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << ErrorReportingTemplate << "\n";
    stream << "#include " << Configuration::inputSrcFilePath() << "\n";
    stream << "#include " << Configuration::outputSrcFilePath() << "\n";
    stream << "#include " << Configuration::algorithmSrcFilePath() << "\n";
    stream << AlgorithmTemplate << "\n";
    stream << MemoryManagementTemplate << "\n";
    return stream.str();
}

std::vector<std::filesystem::path> AlgorithmCompilationInput::inputDependencies()
{
    std::vector<std::filesystem::path> dependencies;
    dependencies.push_back(Configuration::inputSrcFilePath());
    dependencies.push_back(Configuration::outputSrcFilePath());
    dependencies.push_back(Configuration::algorithmSrcFilePath());
    return dependencies;
}