#include "CodeValidator.hpp"

#include <string>

std::string CodeValidator::convert(CXString cxstr)
{
    std::string str = clang_getCString(cxstr);
    clang_disposeString(cxstr);
    return str;
}
