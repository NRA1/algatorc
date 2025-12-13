#include "CompilationInput.hpp"

#include "../Error.hpp"

llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> CompilationInput::inputFileSystem()
{
    buildVFSIfNotBuilt();
    return overlay_.value();
}

bool CompilationInput::compilationNeeded()
{
    if (!std::filesystem::exists(outputFilePath())) return true;

    const std::filesystem::file_time_type output_write_time = std::filesystem::last_write_time(outputFilePath());

    const std::vector dependencies = inputDependencies();
    std::filesystem::file_time_type max_input_write_time = std::filesystem::file_time_type::min();
    for (const std::filesystem::path& dependency : dependencies)
    {
        std::filesystem::file_time_type write_time = std::filesystem::last_write_time(dependency);
        if (write_time > max_input_write_time)
            max_input_write_time = write_time;
    }

    return max_input_write_time >= output_write_time;
}

void CompilationInput::clean()
{
    if (std::filesystem::exists(objFilePath()))
        std::filesystem::remove(objFilePath());
}

void CompilationInput::buildVFSIfNotBuilt()
{
    input_file_ = buildInputFile();
    const llvm::StringRef input_ref(input_file_.value());
    std::unique_ptr<llvm::MemoryBuffer> input_buffer = llvm::MemoryBuffer::getMemBuffer(input_ref);

    llvm::IntrusiveRefCntPtr memory_fs = llvm::IntrusiveRefCntPtr(new llvm::vfs::InMemoryFileSystem());
    memory_fs->addFile(inputFilePath().string(), clock(), std::move(input_buffer));
    memory_fs_ = memory_fs;

    llvm::IntrusiveRefCntPtr overlay = llvm::IntrusiveRefCntPtr(new llvm::vfs::OverlayFileSystem(llvm::vfs::getRealFileSystem()));
    overlay->pushOverlay(memory_fs);

    if (overlay->setCurrentWorkingDirectory(inputFilePath().remove_filename().string()))
        error(ErrorType::System, ErrorPhase::Compilation) << "Failed to set VFS cwd";
    overlay_ = overlay;
}
