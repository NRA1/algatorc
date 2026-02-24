#include "TranslationUnit.hpp"

#include <clang-c/Index.h>

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

bool TranslationUnit::contains(const CodeValidator& validator) const
{
    const CXCursor cursor = clang_getTranslationUnitCursor(unit_);
    const bool found = clang_visitChildren(cursor, [](CXCursor c, CXCursor, void* v)
    {
        const CodeValidator* val = static_cast<CodeValidator*>(v);
        if (val->validate(c)) return CXChildVisit_Break;
        return CXChildVisit_Continue;
    }, const_cast<CodeValidator*>(&validator));

    return found;
}

TranslationUnit::~TranslationUnit()
{
    clang_disposeTranslationUnit(unit_);
    clang_disposeIndex(index_);
}