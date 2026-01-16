#include "Compiler.hpp"

#include <ranges>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/Utils.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <llvm/TargetParser/Host.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/PreprocessorOptions.h>

#include "CompilationDiagnosticsHandler.hpp"
#include "DummyConsumer.hpp"
#include "../Support/Error.hpp"

Compiler::Compiler()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeNativeTargetAsmPrinter();

    const llvm::ErrorOr<std::string> clang_path = llvm::sys::findProgramByName(CLANG_BINARY_NAME);
    if (const std::error_code ec = clang_path.getError())
        error(ErrorType::System, ErrorPhase::Compilation) << "Failed to find clang executable: " << ec.message();

    std::vector<std::string> default_args;
    default_args.push_back(clang_path.get());

    default_args.emplace_back("-shared");
    default_args.emplace_back("-fPIC");
#ifndef NDEBUG
    default_args.emplace_back("-O0");
#endif
#ifdef ALGATORCPP
    default_args.emplace_back("-DALGATORCPP");
    default_args.emplace_back("-std=c++23");
    default_args.emplace_back("-stdlib=libstdc++");
    default_args.emplace_back("-xc++");
#endif
#ifdef ALGATORC
    default_args.emplace_back("-DALGATORC");
    default_args.emplace_back("-std=c23");
    default_args.emplace_back("-xc");
#endif

    default_args_ = default_args;
}

void Compiler::compile(CompilationInput& input)
{
    input.writeInputFile();

    const llvm::ErrorOr<std::string> clang_path = llvm::sys::findProgramByName(CLANG_BINARY_NAME);
    if (const std::error_code ec = clang_path.getError())
        error(ErrorType::System, ErrorPhase::Compilation) << "Failed to find clang executable: " << ec.message();

    std::vector<const char *> args;
    for (std::string& arg : default_args_)
        args.push_back(arg.c_str());

    const std::string output_flag = std::string("-o") + input.outputFilePath().c_str();
    args.push_back(output_flag.c_str());

    const std::string input_file = input.inputFilePath();
    args.push_back(input_file.c_str());

    // ReSharper disable once CppDFAMemoryLeak // Deleted by DiagnosticsEngine
    CompilationDiagnosticsHandler* diagnostics_handler = new CompilationDiagnosticsHandler();
    const llvm::IntrusiveRefCntPtr diagnostic_ids(new clang::DiagnosticIDs());
    const llvm::IntrusiveRefCntPtr diagnostic_engine(new clang::DiagnosticsEngine(diagnostic_ids, diagnostic_options_, diagnostics_handler));

    clang::driver::Driver driver(clang_path.get(), llvm::sys::getDefaultTargetTriple(), *diagnostic_engine, "ALGatorC Clang");

    clang::driver::Compilation* compilation = driver.BuildCompilation(args);
    if (compilation)
    {
        llvm::SmallVector<std::pair<int, const clang::driver::Command*>> failing_command;
        const int res = driver.ExecuteCompilation(*compilation, failing_command);
        if (res < 0)
        {
            for (const clang::driver::Command*& command : failing_command | std::views::values)
                driver.generateCompilationDiagnostics(*compilation, *command);
            error(ErrorType::System, ErrorPhase::Compilation, "Compilation failed");
        }
    }
    else error(ErrorType::System, ErrorPhase::Compilation, "Failed to build compilation");
}