#include "CompilationDiagnosticsHandler.hpp"

#include <iostream>
#include <clang/Lex/Preprocessor.h>

void CompilationDiagnosticsHandler::BeginSourceFile(const clang::LangOptions& LangOpts, const clang::Preprocessor* PP)
{
    std::cout << "Starting processing source file" << std::endl;
    DiagnosticConsumer::BeginSourceFile(LangOpts, PP);
}

void CompilationDiagnosticsHandler::EndSourceFile()
{
    std::cout << "Ending processing source file" << std::endl;
    DiagnosticConsumer::EndSourceFile();
}

void CompilationDiagnosticsHandler::finish()
{
    std::cout << "Compilation finished" << std::endl;
    DiagnosticConsumer::finish();
}

void CompilationDiagnosticsHandler::HandleDiagnostic(const clang::DiagnosticsEngine::Level DiagLevel,
                                                     const clang::Diagnostic& Info)
{
    llvm::SmallVector<char> buffer;
    Info.FormatDiagnostic(buffer);
    const std::string str(buffer.data());

    std::cout << DiagLevel << " " << str << std::endl;
    DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
}