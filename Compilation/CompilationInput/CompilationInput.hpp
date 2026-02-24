#ifndef ALGATORC_COMPILATIONINPUT_HPP
#define ALGATORC_COMPILATIONINPUT_HPP

#include <filesystem>
#include <llvm/Support/VirtualFileSystem.h>

#include "../../Execution/DynamicLibrary.hpp"


class CompilationInput
{
public:
    virtual std::filesystem::path inputFilePath() = 0;
    virtual std::filesystem::path outputFilePath() = 0;
    virtual std::vector<std::string> compilationDefines();

    std::filesystem::path objFilePath();

    void writeInputFile();

    bool compilationNeeded();

    void clean();

    virtual ~CompilationInput() = default;

protected:
    virtual std::string buildInputFile() = 0;
    virtual std::vector<std::filesystem::path> inputDependencies() = 0;

    template<typename T>
    T loadDynamicLibrary();

private:
    std::optional<std::string> input_file_;
};

template <typename T>
T CompilationInput::loadDynamicLibrary()
{
    const std::filesystem::path path = outputFilePath();
    if (!std::filesystem::exists(path))
    {
        error(ErrorType::System, "Tried to load dynamic library from path ") << path.string() << " but the path does not exist";
    }

    return T(path);
}

#endif //ALGATORC_COMPILATIONINPUT_HPP
