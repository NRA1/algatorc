#ifndef ALGATORC_TRANSLATIONINPUT_HPP
#define ALGATORC_TRANSLATIONINPUT_HPP
#include <filesystem>
#include <string>


class TranslationInput
{
public:
    virtual const std::filesystem::path& sourcePath() const = 0;
    virtual std::string inputSource() = 0;

    virtual ~TranslationInput() = default;
};


#endif //ALGATORC_TRANSLATIONINPUT_HPP