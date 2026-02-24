#ifndef ALGATORC_INPUTTRANSLATIONINPUT_HPP
#define ALGATORC_INPUTTRANSLATIONINPUT_HPP
#include "TranslationInput.hpp"


class InputTranslationInput : public TranslationInput
{
public:
    InputTranslationInput() = default;

    const std::filesystem::path& sourcePath() const override;
    std::string inputSource() override;
};


#endif //ALGATORC_INPUTTRANSLATIONINPUT_HPP