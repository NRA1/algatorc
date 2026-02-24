#include "AlgorithmTranslationInput.hpp"

#include "../../Support/Configuration.hpp"
#include "../../Support/FileManagement.hpp"

const std::filesystem::path& AlgorithmTranslationInput::sourcePath() const
{
    return Configuration::algorithmSrcFilePath();
}

std::string AlgorithmTranslationInput::inputSource()
{
    const std::string source = readTextFile(sourcePath());
    const std::string input = "struct output; struct input;";
    return input + source;

}
