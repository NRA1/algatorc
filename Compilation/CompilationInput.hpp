#ifndef ALGATORC_COMPILATIONINPUT_HPP
#define ALGATORC_COMPILATIONINPUT_HPP

#include <filesystem>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/VirtualFileSystem.h>


class CompilationInput
{
public:
    virtual std::filesystem::path inputFilePath() = 0;
    virtual std::filesystem::path objFilePath() = 0;
    virtual std::filesystem::path outputFilePath() = 0;

    llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> inputFileSystem();

    bool compilationNeeded();

    void clean();

    virtual ~CompilationInput() = default;

protected:
    virtual std::string buildInputFile() = 0;
    virtual std::vector<std::filesystem::path> inputDependencies() = 0;

private:
    void buildVFSIfNotBuilt();

    std::optional<llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem>> memory_fs_;
    std::optional<llvm::IntrusiveRefCntPtr<llvm::vfs::OverlayFileSystem>> overlay_;

    std::optional<std::string> input_file_;
};

#endif //ALGATORC_COMPILATIONINPUT_HPP
