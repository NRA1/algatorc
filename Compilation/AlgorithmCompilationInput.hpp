#ifndef ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP
#define ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP
#include "CompilationInput.hpp"
#include "../Configuration.hpp"


class AlgorithmCompilationInput final : public CompilationInput
{
public:
    explicit AlgorithmCompilationInput(Configuration& config);

    std::filesystem::path inputFilePath() override;
    std::filesystem::path objFilePath() override;
    std::filesystem::path outputFilePath() override;

protected:
    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;

private:
    Configuration& config_;
};


#endif //ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP