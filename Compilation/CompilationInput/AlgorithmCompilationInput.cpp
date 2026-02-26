#include "AlgorithmCompilationInput.hpp"

#include "../../Support/Error.hpp"
#include <Templates/Algorithm.hpp>
#include <Templates/ErrorReporting.hpp>

#include "../TranslationUnit.hpp"
#include "../CodeValidator/ExecuteValidator.hpp"
#include "../TranslationInput/AlgorithmTranslationInput.hpp"

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
        error(ErrorType::System, "Failed to create binary directories: ") << e.what();
    }

    validateSourceFiles();
}

std::filesystem::path AlgorithmCompilationInput::inputFilePath()
{
    return Configuration::temporaryDir() / "algorithm" SOURCE_EXTENSION;
}

std::filesystem::path AlgorithmCompilationInput::outputFilePath()
{
    return Configuration::algorithmBinDir() / "algorithm" SOURCE_EXTENSION DYLIB_EXTENSION;
}

void AlgorithmCompilationInput::validateSourceFiles()
{
    AlgorithmTranslationInput input{};
    const TranslationUnit unit{input};

    if (!unit.contains(ExecuteValidator{}))
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "\"execute\" function not defined. Define algorithm entry point with format"
                               " \"output* execute(input*)\" in \"algorithm" SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "\"execute\" function not defined. Define algorithm entry point with format"
                               " \"struct output* execute(struct input*)\" in \"algorithm" SOURCE_EXTENSION "\".");
#endif
    }
}

std::string AlgorithmCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << "#include " << Configuration::inputSrcFilePath() << "\n";
    stream << "#include " << Configuration::outputSrcFilePath() << "\n";
    stream << "#include " << Configuration::algorithmSrcFilePath() << "\n";
    stream << ErrorReportingTemplate << "\n";
    stream << AlgorithmTemplate << "\n";
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

long long getHits()
{
    return -1;
}