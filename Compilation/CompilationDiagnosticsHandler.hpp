#ifndef ALGATORC_COMPILATION_DIAGNOSTIC_HANDLER_HPP
#define ALGATORC_COMPILATION_DIAGNOSTIC_HANDLER_HPP
#include <clang/Basic/Diagnostic.h>


class CompilationDiagnosticsHandler final : public clang::DiagnosticConsumer
{
public:
    void BeginSourceFile(const clang::LangOptions& LangOpts, const clang::Preprocessor* PP) override;
    void EndSourceFile() override;
    void finish() override;
    void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel, const clang::Diagnostic& Info) override;
};


#endif //ALGATORC_COMPILATION_DIAGNOSTIC_HANDLER_HPP