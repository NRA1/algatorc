#ifndef ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#define ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#include "CompilationInput.hpp"
#include "../Support/Configuration.hpp"


class ProjectCompilationInput final : public CompilationInput
{
public:
    explicit ProjectCompilationInput(Configuration& config);

    std::filesystem::path inputFilePath() override;
    std::filesystem::path outputFilePath() override;

protected:
    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;

private:
    Configuration& config_;
};


#endif //ALGATORC_PROJECTCOMPILATIONINPUT_HPP