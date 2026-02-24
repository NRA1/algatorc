#ifndef ALGATORC_PARSER_HPP
#define ALGATORC_PARSER_HPP
#include <filesystem>
#include <clang-c/Index.h>

#include "CodeValidator/FunctionValidator.hpp"
#include "TranslationInput/TranslationInput.hpp"


class TranslationUnit
{
public:
    explicit TranslationUnit(TranslationInput& input);

    bool contains(const CodeValidator& validator) const;

    ~TranslationUnit();

private:
    CXIndex index_;
    CXTranslationUnit unit_;
};


#endif //ALGATORC_PARSER_HPP