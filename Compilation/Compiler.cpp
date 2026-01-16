#include "Compiler.hpp"

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

    std::vector<std::string> default_args = getDriverArgs();
    // default_args.push_back(clang_path.get());

    // default_args.emplace_back("-emit-obj");
    default_args.emplace_back("-clear-ast-before-backend");
    default_args.emplace_back("-fcxx-exceptions");
    default_args.emplace_back("-fexceptions");
    default_args.emplace_back("-faddrsig");
    default_args.emplace_back("-D__GCC_HAVE_DWARF2_CFI_ASM=1");
#ifndef NDEBUG
    // default_args.emplace_back("-g");
    default_args.emplace_back("-O0");
#endif
#ifdef ALGATORCPP
    default_args.emplace_back("-DALGATORCPP");
    default_args.emplace_back("-std=c++23");
    default_args.emplace_back("-stdlib=libstdc++");
    default_args.emplace_back("-fexceptions");
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
    std::vector<const char *> args;
    for (std::string& arg : default_args_)
        args.push_back(arg.c_str());

    const std::string input_file = input.inputFilePath();
    args.push_back(input_file.c_str());

    // ReSharper disable once CppDFAMemoryLeak // Deleted by DiagnosticsEngine
    // CompilationDiagnosticsHandler* diagnostics_handler = new CompilationDiagnosticsHandler();
    clang::TextDiagnosticPrinter *printer = new clang::TextDiagnosticPrinter(llvm::errs(), diagnostic_options_);
    const llvm::IntrusiveRefCntPtr diagnostic_ids(new clang::DiagnosticIDs());
    const llvm::IntrusiveRefCntPtr diagnostic_engine(new clang::DiagnosticsEngine(diagnostic_ids, diagnostic_options_, printer));

    // clang::CreateInvocationOptions invocation_options;
    // invocation_options.Diags = diagnostic_engine;
    // invocation_options.VFS = input.inputFileSystem();
    // const std::shared_ptr invocation = clang::createInvocation(args, invocation_options);
    const std::shared_ptr invocation = std::make_shared<clang::CompilerInvocation>();
    clang::CompilerInvocation::CreateFromArgs(*invocation, args, *diagnostic_engine);


//     clang::PreprocessorOptions& preprocessor_opts = invocation->getPreprocessorOpts();
//     preprocessor_opts.UsePredefines = true;
//
//     clang::LangOptions& lang_options = invocation->getLangOpts();
//     lang_options.GNUMode = 1;
//     lang_options.GNUKeywords = 1;
//     lang_options.WChar = 1;
//     lang_options.C99 = 1;
//     lang_options.PICLevel = 2;
//     lang_options.DoubleSquareBracketAttributes = 1;
// #ifdef ALGATORCPP
//     lang_options.CXXOperatorNames = 1;
//     lang_options.Exceptions = 1;
//     lang_options.ObjCExceptions = 1;
//     lang_options.CXXExceptions = 1;
//     // lang_options.EHAsynch = 1;
//     lang_options.ExternCNoUnwind = 0;
//     lang_options.MicrosoftExt = 0;
//     lang_options.RTTI = 0;
//     lang_options.RTTIData = 0;
//
//     lang_options.setExceptionHandling(clang::LangOptionsBase::ExceptionHandlingKind::DwarfCFI);
//     lang_options.IgnoreExceptions = 0;
//     lang_options.AssumeNothrowExceptionDtor = 0;
//     lang_options.Bool = 1;
//     lang_options.CPlusPlus = 1;
//     lang_options.CPlusPlus11 = 1;
//     lang_options.CPlusPlus14 = 1;
//     lang_options.CPlusPlus17 = 1;
//     lang_options.CPlusPlus20 = 1;
//     lang_options.CPlusPlus23 = 1;
//     // lang_options.setExceptionHandling(clang::LangOptionsBase::ExceptionHandlingKind::DwarfCFI);
//     lang_options.setFPExceptionMode(clang::LangOptionsBase::FPExceptionModeKind::FPE_Strict);
//     lang_options.LangStd = clang::LangStandard::Kind::lang_cxx23;
// #endif
// #ifdef ALGATORC
//     lang_options.CXXExceptions = 0;
//     lang_options.Bool = 0;
//     lang_options.CPlusPlus = 0;
//     lang_options.CPlusPlus11 = 0;
//     lang_options.CPlusPlus14 = 0;
//     lang_options.CPlusPlus17 = 0;
//     lang_options.CPlusPlus20 = 0;
//     lang_options.CPlusPlus23 = 0;
//     lang_options.LangStd = clang::LangStandard::Kind::lang_c23;
// #endif
//
//     clang::HeaderSearchOptions& header_search_opts = invocation->getHeaderSearchOpts();
//     header_search_opts.UseStandardSystemIncludes = 1;
//     header_search_opts.UseStandardCXXIncludes = 1;
//
//     clang::CodeGenOptions& opts = invocation->getCodeGenOpts();
//     opts.DwarfVersion = 2;
//     opts.StackRealignment = 1;

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

std::vector<std::string> Compiler::getDriverArgs()
{
    clang::DiagnosticOptions diagnostic_options{};
    // ReSharper disable once CppDFAMemoryLeak // Deleted by DiagnosticEngine
    DummyConsumer* consumer = new DummyConsumer();
    const llvm::IntrusiveRefCntPtr diagnostic_ids(new clang::DiagnosticIDs());
    const llvm::IntrusiveRefCntPtr driver_diagnostic_engine(new clang::DiagnosticsEngine(diagnostic_ids, diagnostic_options, consumer));

    const llvm::ErrorOr<std::string> clang_path = llvm::sys::findProgramByName(CLANG_BINARY_NAME);
    if (const std::error_code ec = clang_path.getError())
        error(ErrorType::System, ErrorPhase::Compilation) << "Failed to find clang executable: " << ec.message();

    std::vector<const char*> driver_args;
    driver_args.push_back(clang_path->c_str());
    driver_args.push_back("-c");
    driver_args.push_back("in.cpp");
    driver_args.push_back("-shared");
    driver_args.push_back("-fPIC");
    clang::driver::Driver driver(clang_path.get(), llvm::sys::getDefaultTargetTriple(), *driver_diagnostic_engine, "ALGatorC Clang");
    driver.setCheckInputsExist(false);
    const clang::driver::Compilation* compilation = driver.BuildCompilation(driver_args);
    if (!compilation) error(ErrorType::System, ErrorPhase::Compilation) << "Driver compilation arguments query failed";

    // ReSharper disable once CppDFANullDereference // error is noreturn
    const clang::driver::JobList& jobs = compilation->getJobs();
    if (jobs.size() != 1)
        error(ErrorType::System, ErrorPhase::Compilation, "Driver compilation arguments query failed");
    const llvm::opt::ArgStringList& args_list = jobs.getJobs()[0].get()->getArguments();
    std::vector<std::string> args;
    // for (const char* const& arg : args_list)
    // {
    //     if (strcmp(arg, "-o") == 0 || strcmp(arg, "in.cpp") == 0 || strcmp(arg, "in.o") == 0
    //         || strcmp(arg, "-cc1") == 0) continue;
    //     if (arg[0] != '-') args[args.size() - 1] += std::string("=") + arg;
    //     else args.push_back(arg);
    // }
    for (int i = 0; i < args_list.size() - 1; i++)
    {
        if (strcmp(args_list[i], "-o") == 0 || strcmp(args_list[i], "in.cpp") == 0 || strcmp(args_list[i], "in.o") == 0
            || strcmp(args_list[i], "-main-file-name") == 0) continue;
        args.push_back(args_list[i]);
        continue;
        if (
            strcmp(args_list[i], "-internal-isystem") == 0 ||
            strcmp(args_list[i], "-internal-externc-isystem") == 0 ||
            strcmp(args_list[i], "-isystem") == 0 ||
            strcmp(args_list[i], "-resource-dir") == 0
            )
        {
            args.push_back(args_list[i]);
            args.push_back(args_list[i + 1]);
        }
    }
    return args;
}
