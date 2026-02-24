#ifndef ALGATORC_CODEVALIDATOR_HPP
#define ALGATORC_CODEVALIDATOR_HPP
#include <string>
#include <clang-c/Index.h>


class CodeValidator
{
public:
    virtual bool validate(const CXCursor& cursor) const = 0;

    virtual ~CodeValidator() = default;

protected:
    static std::string convert(CXString cxstr);
};


#endif //ALGATORC_CODEVALIDATOR_HPP