#ifndef ALGATORC_OUTPUTTRANSLATIONINPUT_HPP
#define ALGATORC_OUTPUTTRANSLATIONINPUT_HPP
#include "TranslationInput.hpp"


class OutputTranslationInput : public TranslationInput
{
public:
    OutputTranslationInput() = default;

    const std::filesystem::path& sourcePath() const override;
    std::string inputSource() override;
};


#endif //ALGATORC_OUTPUTTRANSLATIONINPUT_HPP