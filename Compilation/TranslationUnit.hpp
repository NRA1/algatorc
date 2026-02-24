#ifndef ALGATORC_PARSER_HPP
#define ALGATORC_PARSER_HPP
#include <filesystem>
#include <clang-c/Index.h>

#include "FunctionValidator/FunctionValidator.hpp"
#include "TranslationInput/TranslationInput.hpp"


class TranslationUnit
{
public:
    explicit TranslationUnit(TranslationInput& input);

    bool contains(const FunctionValidator& validator) const;

    ~TranslationUnit();

private:
    static std::string convert(CXString cxstr);

    CXIndex index_;
    CXTranslationUnit unit_;
};


#endif //ALGATORC_PARSER_HPP