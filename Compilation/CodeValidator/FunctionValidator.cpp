#include "FunctionValidator.hpp"

bool FunctionValidator::validate(const CXCursor& cursor) const
{
    if (clang_getCursorKind(cursor) != CXCursor_FunctionDecl) return false;

    const std::string name = convert(clang_getCursorSpelling(cursor));
    if (!validateName(name)) return false;

    const CXType type = clang_getCursorType(cursor);
    const CXType return_type = clang_getResultType(type);
    const CXType base_return_type = clang_getUnqualifiedType(return_type);
    const std::string return_type_str = convert(clang_getTypeSpelling(base_return_type));
    if (!validateReturnType(return_type_str)) return false;

    const int num_args = clang_Cursor_getNumArguments(cursor);
    std::vector<std::string> args;
    args.reserve(num_args);
    for (int i = 0; i < num_args; i++)
    {
        const CXType arg_type = clang_getArgType(type, i);
        const CXType base_type = clang_getUnqualifiedType(arg_type);
        std::string arg_type_str = convert(clang_getTypeSpelling(base_type));
        args.push_back(arg_type_str);
    }
    if (!validateArguments(args)) return false;

    return true;
}
