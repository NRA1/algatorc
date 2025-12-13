#include "Compiler.hpp"

#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/Utils.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <llvm/TargetParser/Host.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#include "../Error.hpp"

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
    default_args.emplace_back("-std=c++23");
    default_args.emplace_back("-stdlib=libstdc++");
    default_args.emplace_back("-fPIC");
    default_args.emplace_back("-xc++");

    default_args_ = default_args;
}

void Compiler::compile(CompilationInput& input)
{
    std::vector<const char *> args;
    for (std::string& arg : default_args_)
        args.push_back(arg.c_str());

    const std::string output_arg = std::string("-o") + input.objFilePath().string();
    args.push_back(output_arg.c_str());

    const std::string input_file = input.inputFilePath();
    args.push_back(input_file.c_str());


    // ReSharper disable once CppDFAMemoryLeak // Deleted by DiagnosticsEngine
    // CompilationDiagnosticsHandler* diagnostics_handler = new CompilationDiagnosticsHandler();
    clang::TextDiagnosticPrinter *printer = new clang::TextDiagnosticPrinter(llvm::errs(), diagnostic_options_);
    const llvm::IntrusiveRefCntPtr diagnostic_ids(new clang::DiagnosticIDs());
    const llvm::IntrusiveRefCntPtr diagnostic_engine(new clang::DiagnosticsEngine(diagnostic_ids, diagnostic_options_, printer));

    clang::CreateInvocationOptions invocation_options;
    invocation_options.Diags = diagnostic_engine;
    invocation_options.VFS = input.inputFileSystem();
    const std::shared_ptr invocation = clang::createInvocation(args, invocation_options);

    clang::LangOptions& lang_options = invocation->getLangOpts();
    lang_options.CXXExceptions = 1;
    lang_options.Bool = 1;
    lang_options.CPlusPlus = 1;
    lang_options.CPlusPlus11 = 1;
    lang_options.CPlusPlus14 = 1;
    lang_options.CPlusPlus17 = 1;
    lang_options.CPlusPlus20 = 1;
    lang_options.CPlusPlus23 = 1;
    lang_options.LangStd = clang::LangStandard::Kind::lang_cxx23;

    clang::CompilerInstance instance(invocation);
    instance.setDiagnostics(&*diagnostic_engine);

    clang::TargetOptions target_options{};
    target_options.Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo* target_info = clang::TargetInfo::CreateTargetInfo(*diagnostic_engine,target_options);
    instance.setTarget(target_info);

    const auto fm = instance.createFileManager(input.inputFileSystem());
    instance.createSourceManager(*fm);

    clang::EmitObjAction compilerAction{};
    if (!instance.ExecuteAction(compilerAction))
        error(ErrorType::System, ErrorPhase::Compilation, "Compilation failed");
}
