#ifndef ALGATORC_ALGORITHMTRANSLATIONINPUT_HPP
#define ALGATORC_ALGORITHMTRANSLATIONINPUT_HPP
#include "TranslationInput.hpp"


class AlgorithmTranslationInput : public TranslationInput
{
public:
    AlgorithmTranslationInput() = default;

    const std::filesystem::path& sourcePath() const override;
    std::string inputSource() override;
};


#endif //ALGATORC_ALGORITHMTRANSLATIONINPUT_HPP