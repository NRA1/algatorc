#include "DataConverterTranslationInput.hpp"

#include "../../Support/Configuration.hpp"
#include "../../Support/FileManagement.hpp"

const std::filesystem::path& DataConverterTranslationInput::sourcePath() const
{
    return Configuration::dataConverterSrcFilePath();
}

std::string DataConverterTranslationInput::inputSource()
{
    const std::string source = readTextFile(sourcePath());
    std::string input = "struct output; struct input;\n";
#ifdef ALGATORCPP
    input += "namespace std { class istream; class ostream; }\n";
#endif
    return input + source;
}
