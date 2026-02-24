#include "ClassValidator.hpp"

bool ClassValidator::validate(const CXCursor& cursor) const
{
    const CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind != CXCursor_StructDecl && kind != CXCursor_ClassDecl) return false;

    const std::string name = convert(clang_getCursorSpelling(cursor));
    if (!validateName(name)) return false;

    return true;
}
