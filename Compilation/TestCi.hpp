#ifndef ALGATORC_TESTCI_HPP
#define ALGATORC_TESTCI_HPP
#include "CompilationInput.hpp"


class Configuration;

class TestCI : public CompilationInput
{
public:
    explicit TestCI(Configuration& config);

    std::filesystem::path inputFilePath() override;
    std::filesystem::path outputFilePath() override;

protected:
    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;

private:
    Configuration& config_;

};


#endif //ALGATORC_TESTCI_HPP