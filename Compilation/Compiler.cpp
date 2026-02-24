#include "Compiler.hpp"

#include <ranges>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInstance.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>

#include "DummyConsumer.hpp"
#include "../Support/Configuration.hpp"
#include "../Support/Error.hpp"
#include "../Support/FileManagement.hpp"
#include <clang/Frontend/TextDiagnosticPrinter.h>

Compiler::Compiler()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeNativeTargetAsmPrinter();

    const llvm::ErrorOr<std::string> clang_path = llvm::sys::findProgramByName(CLANG_BINARY_NAME);
    if (const std::error_code ec = clang_path.getError())
        error(ErrorType::System) << "Failed to find clang executable: " << ec.message();

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
        error(ErrorType::System) << "Failed to find clang executable: " << ec.message();

    std::vector<const char *> args;
    for (std::string& arg : default_args_)
        args.push_back(arg.c_str());

    std::vector<std::string> defines = input.compilationDefines();
    for (int i = 0; i < defines.size(); i++)
    {
        defines[i] = "-D" + defines[i];
        args.push_back(defines[i].c_str());
    }

    const std::string output_flag = std::string("-o") + input.outputFilePath().string();
    args.push_back(output_flag.c_str());

    const std::string input_file = input.inputFilePath().string();
    args.push_back(input_file.c_str());

    // ReSharper disable once CppDFAMemoryLeak // Deleted by DiagnosticsEngine
    // DummyConsumer* diagnostics_handler = new DummyConsumer();
    clang::TextDiagnosticPrinter* diagnostics_handler = new clang::TextDiagnosticPrinter(llvm::errs(), diagnostic_options_);
    const llvm::IntrusiveRefCntPtr diagnostic_ids(new clang::DiagnosticIDs());
    const llvm::IntrusiveRefCntPtr diagnostic_engine(new clang::DiagnosticsEngine(diagnostic_ids, diagnostic_options_, diagnostics_handler));

    clang::driver::Driver driver(clang_path.get(), llvm::sys::getDefaultTargetTriple(), *diagnostic_engine, "ALGatorC Clang");

    clang::driver::Compilation* compilation = driver.BuildCompilation(args);
    if (compilation)
    {
        const std::filesystem::path outfile_path = Configuration::temporaryDir() / "compilation.out";
        std::filesystem::remove(outfile_path);
        const std::string outfile = outfile_path.string();
        compilation->Redirect({std::nullopt, outfile, outfile});

        llvm::SmallVector<std::pair<int, const clang::driver::Command*>> failing_command;
        const int res = driver.ExecuteCompilation(*compilation, failing_command);
        if (res != 0 || !failing_command.empty())
        {
            if (!std::filesystem::exists(outfile_path))
                error(ErrorType::System, "Compilation failed but compilation output file was not created");
            const std::string compilation_output = readTextFile(outfile_path);

            error(ErrorType::User, "Compilation failed:\n") << compilation_output;
        }
        delete compilation;
    }
    else error(ErrorType::System, "Failed to build compilation");
}