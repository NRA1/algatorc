#include "InputTranslationInput.hpp"

#include "../../Support/Configuration.hpp"
#include "../../Support/FileManagement.hpp"

const std::filesystem::path& InputTranslationInput::sourcePath() const
{
    return Configuration::inputSrcFilePath();
}

std::string InputTranslationInput::inputSource()
{
    return readTextFile(sourcePath());
}
