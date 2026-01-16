#include "Linker.hpp"

#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Driver/Driver.h>
#include <lld/Common/Driver.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/Program.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/VirtualFileSystem.h>

#include "Compiler.hpp"
#include "DummyConsumer.hpp"
#include "../Support/Error.hpp"
#include <clang/Driver/Compilation.h>

Linker::Linker()
{
    const std::vector<std::string> search_paths = findLibraryPaths();

    std::vector<std::string> default_args;
    default_args.emplace_back("ld.lld");
    default_args.append_range(getDriverArgs());
    // default_args.emplace_back("-shared");
    //
    // for (auto& path : search_paths)
    //     default_args.push_back(std::string("-L") + path);
    //
    // default_args.emplace_back("-lstdc++");
    // default_args.emplace_back("-lm");
    // default_args.emplace_back("-lgcc");
    // default_args.emplace_back("-lc");
    // default_args.emplace_back("-lgcc_s");

    default_args_ = default_args;

}

void Linker::link(CompilationInput& input)
{
    std::vector<const char*> args;
    for (std::string& arg : default_args_)
        args.push_back(arg.c_str());

    const std::string output_arg = std::string("-o") + input.outputFilePath().string();
    args.push_back(output_arg.c_str());

    const std::string input_path = input.objFilePath();
    args.push_back(input_path.c_str());

    auto [retCode, canRunAgain] = lld::lldMain(args, llvm::outs(), llvm::errs(), {{lld::Gnu, &lld::elf::link}});
    if (retCode != 0 || canRunAgain != true)
        error(ErrorType::System, ErrorPhase::Compilation, "Linker did not exit successfully");
}

std::vector<std::string> Linker::findLibraryPaths()
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
    clang::driver::Driver driver(clang_path.get(), llvm::sys::getDefaultTargetTriple(), *driver_diagnostic_engine, "ALGatorC Clang");
    driver.setCheckInputsExist(false);
    const clang::driver::Compilation* compilation = driver.BuildCompilation(driver_args);
    if (!compilation) error(ErrorType::System, ErrorPhase::Compilation) << "Library search directories query failed";

    // ReSharper disable once CppDFANullDereference
    const clang::driver::ToolChain& toolchain = compilation->getDefaultToolChain();
    const llvm::SmallVector<std::string, 16>& search_paths = toolchain.getFilePaths();

    std::vector<std::string> search_paths_vec;
    for (auto& path : search_paths)
        search_paths_vec.push_back(path);

    return search_paths_vec;
}

std::vector<std::string> Linker::getDriverArgs()
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
    // driver_args.push_back("-c");
    driver_args.push_back("in.o");
    driver_args.push_back("-shared");
    clang::driver::Driver driver(clang_path.get(), llvm::sys::getDefaultTargetTriple(), *driver_diagnostic_engine, "ALGatorC LLD");
    driver.setCheckInputsExist(false);
    const clang::driver::Compilation* compilation = driver.BuildCompilation(driver_args);
    if (!compilation) error(ErrorType::System, ErrorPhase::Compilation) << "Driver linking arguments query failed";

    // ReSharper disable once CppDFANullDereference // error is noreturn
    const clang::driver::JobList& jobs = compilation->getJobs();
    if (jobs.size() != 1)
        error(ErrorType::System, ErrorPhase::Compilation, "Driver linking arguments query failed");
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
        if (strcmp(args_list[i], "-o") == 0 || strcmp(args_list[i], "a.out") == 0
            || strcmp(args_list[i], "in.o") == 0
            || strcmp(args_list[i], "-lstdc++") == 0
            || strcmp(args_list[i], "-lgcc_s") == 0
            || strcmp(args_list[i], "-lm") == 0

        ) continue;
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
