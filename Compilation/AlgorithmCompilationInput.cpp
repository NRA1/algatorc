#include "AlgorithmCompilationInput.hpp"

#include "../Support/Error.hpp"
#include <Templates/Algorithm.hpp>
#include <Templates/ErrorReporting.hpp>

AlgorithmCompilationInput::AlgorithmCompilationInput(Configuration& config) : config_(config)
{
    try
    {
        if (!std::filesystem::exists(config.temporaryDir()))
            std::filesystem::create_directories(config.temporaryDir());

        if (!std::filesystem::exists(config.algorithmBinDir()))
            std::filesystem::create_directories(config.algorithmBinDir());
    }
    catch (std::filesystem::filesystem_error& e)
    {
        error(ErrorType::System, ErrorPhase::Preparation, "Failed to create binary directories: ") << e.what();
    }
}

std::filesystem::path AlgorithmCompilationInput::inputFilePath()
{
    return config_.temporaryDir() / "algorithm" SOURCE_EXTENSION;
}

std::filesystem::path AlgorithmCompilationInput::outputFilePath()
{
    return config_.algorithmBinDir() / "algorithm" SOURCE_EXTENSION ".so";
}

std::string AlgorithmCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << ErrorReportingTemplate << "\n";
    stream << "#include " << config_.inputSrcFilePath() << "\n";
    stream << "#include " << config_.outputSrcFilePath() << "\n";
    stream << "#include " << config_.algorithmSrcFilePath() << "\n";
    stream << AlgorithmTemplate << "\n";
    return stream.str();
}

std::vector<std::filesystem::path> AlgorithmCompilationInput::inputDependencies()
{
    std::vector<std::filesystem::path> dependencies;
    dependencies.push_back(config_.inputSrcFilePath());
    dependencies.push_back(config_.outputSrcFilePath());
    dependencies.push_back(config_.algorithmSrcFilePath());
    return dependencies;
}