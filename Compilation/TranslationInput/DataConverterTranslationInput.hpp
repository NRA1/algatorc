#ifndef ALGATORC_DATACONVERTERTRANSLATIONINPUT_HPP
#define ALGATORC_DATACONVERTERTRANSLATIONINPUT_HPP
#include "TranslationInput.hpp"


class DataConverterTranslationInput : public TranslationInput
{
public:
    DataConverterTranslationInput();

    const std::filesystem::path& sourcePath() const;
    std::string inputSource() override;
};


#endif //ALGATORC_DATACONVERTERTRANSLATIONINPUT_HPP