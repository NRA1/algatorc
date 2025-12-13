#include "ProjectCompilationInput.hpp"

#include "../Error.hpp"

ProjectCompilationInput::ProjectCompilationInput(Configuration& config) : config_(config)
{
    try
    {
        if (!std::filesystem::exists(config.temporaryDir()))
            std::filesystem::create_directories(config.temporaryDir());

        if (!std::filesystem::exists(config.projectBinDir()))
            std::filesystem::create_directories(config.projectBinDir());
    }
    catch (std::filesystem::filesystem_error& e)
    {
        error(ErrorType::System, ErrorPhase::Preparation, "Failed to create binary directories: ") << e.what();
    }
}

std::filesystem::path ProjectCompilationInput::inputFilePath()
{
    return config_.temporaryDir() / "project.cpp";
}

std::filesystem::path ProjectCompilationInput::objFilePath()
{
    return config_.temporaryDir() / "project.o";
}

std::filesystem::path ProjectCompilationInput::outputFilePath()
{
    return config_.projectBinDir() / "project.so";
}

std::string ProjectCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << "#include " << config_.inputSrcFilePath() << "\n";
    stream << "#include " << config_.outputSrcFilePath() << "\n";
    stream << "#include " << config_.dataConverterSrcFilePath() << "\n";
    return stream.str();
}

std::vector<std::filesystem::path> ProjectCompilationInput::inputDependencies()
{
    std::vector<std::filesystem::path> dependencies;
    dependencies.push_back(config_.inputSrcFilePath());
    dependencies.push_back(config_.outputSrcFilePath());
    dependencies.push_back(config_.dataConverterSrcFilePath());
    return dependencies;
}
