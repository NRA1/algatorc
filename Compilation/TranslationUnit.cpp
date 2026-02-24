#include "TranslationUnit.hpp"

#include <clang-c/Index.h>
#include <vector>

#include "../Support/Configuration.hpp"
#include "../Support/Error.hpp"
#include "TranslationInput/TranslationInput.hpp"

TranslationUnit::TranslationUnit(TranslationInput& input)
{
    const std::string path = input.sourcePath().string();
    const std::string input_source = input.inputSource();
    CXUnsavedFile file
    {
        path.c_str(),
        input_source.c_str(),
        input_source.size()
    };

    index_ = clang_createIndex(0, 0);
    const CXErrorCode code = clang_parseTranslationUnit2(index_, path.c_str(),
        nullptr, 0, &file, 1, CXTranslationUnit_None, &unit_);

    if (code != CXErrorCode::CXError_Success)
    {
        error(ErrorType::System, "Failed to parse translation unit");
    }
}

bool TranslationUnit::contains(const FunctionValidator& validator) const
{
    const CXCursor cursor = clang_getTranslationUnitCursor(unit_);
    const bool found = clang_visitChildren(cursor, [](CXCursor c, CXCursor, void* v)
    {
        if (clang_getCursorKind(c) == CXCursor_FunctionDecl)
        {
            const FunctionValidator* validator = static_cast<FunctionValidator*>(v);
            const std::string name = convert(clang_getCursorSpelling(c));
            if (!validator->validateName(name)) return CXChildVisit_Continue;

            const CXType type = clang_getCursorType(c);
            const CXType return_type = clang_getResultType(type);
            const CXType base_return_type = clang_getUnqualifiedType(return_type);
            const std::string return_type_str = convert(clang_getTypeSpelling(base_return_type));
            if (!validator->validateReturnType(return_type_str)) return CXChildVisit_Continue;

            const int num_args = clang_Cursor_getNumArguments(c);
            std::vector<std::string> args;
            args.reserve(num_args);
            for (int i = 0; i < num_args; i++)
            {
                const CXType arg_type = clang_getArgType(type, i);
                const CXType base_type = clang_getUnqualifiedType(arg_type);
                std::string arg_type_str = convert(clang_getTypeSpelling(base_type));
                args.push_back(arg_type_str);
            }
            if (!validator->validateArguments(args)) return CXChildVisit_Continue;

            return CXChildVisit_Break;
        }

        return CXChildVisit_Continue;
    }, const_cast<FunctionValidator*>(&validator));

    return found;
}

TranslationUnit::~TranslationUnit()
{
    clang_disposeTranslationUnit(unit_);
    clang_disposeIndex(index_);
}

std::string TranslationUnit::convert(CXString cxstr)
{
    std::string str = clang_getCString(cxstr);
    clang_disposeString(cxstr);
    return str;
}