#ifndef ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP
#define ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP
#include "CompilationInput.hpp"
#include "../Loading/AlgorithmLibrary.hpp"
#include "../Support/Configuration.hpp"


class AlgorithmCompilationInput final : public CompilationInput
{
public:
    explicit AlgorithmCompilationInput();

    std::filesystem::path inputFilePath() override;
    std::filesystem::path outputFilePath() override;

    AlgorithmLibrary loadDynamicLibrary();

    std::vector<std::string> wrappedSymbols() const override;

protected:
    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;
};


#endif //ALGATORC_ALGORITHMCOMPILATIONINPUT_HPP