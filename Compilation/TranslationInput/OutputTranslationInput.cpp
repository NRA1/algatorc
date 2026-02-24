#include "OutputTranslationInput.hpp"

#include "../../Support/Configuration.hpp"
#include "../../Support/FileManagement.hpp"

const std::filesystem::path& OutputTranslationInput::sourcePath() const
{
    return Configuration::outputSrcFilePath();
}

std::string OutputTranslationInput::inputSource()
{
    return readTextFile(sourcePath());
}
