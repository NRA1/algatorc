#include "DataConverterTranslationInput.hpp"

#include "../../Support/Configuration.hpp"
#include "../../Support/FileManagement.hpp"

DataConverterTranslationInput::DataConverterTranslationInput()
{
}

const std::filesystem::path& DataConverterTranslationInput::sourcePath() const
{
    return Configuration::dataConverterSrcFilePath();
}

std::string DataConverterTranslationInput::inputSource()
{
    const std::string source = readTextFile(Configuration::dataConverterSrcFilePath());
    const std::string input = "struct output; struct input;";
    return input + source;
}
