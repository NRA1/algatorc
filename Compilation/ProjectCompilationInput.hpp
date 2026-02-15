#ifndef ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#define ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#include "CompilationInput.hpp"
#include "../Execution/ProjectLibrary.hpp"
#include "../Support/Configuration.hpp"


class ProjectCompilationInput final : public CompilationInput
{
public:
    explicit ProjectCompilationInput();

    std::filesystem::path inputFilePath() override;
    std::filesystem::path outputFilePath() override;

    ProjectLibrary loadDynamicLibrary();

protected:
    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;
};


#endif //ALGATORC_PROJECTCOMPILATIONINPUT_HPP