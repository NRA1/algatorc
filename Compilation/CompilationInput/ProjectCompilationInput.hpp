#ifndef ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#define ALGATORC_PROJECTCOMPILATIONINPUT_HPP
#include "CompilationInput.hpp"
#include "../../Execution/ProjectLibrary.hpp"
#include "../../Support/Configuration.hpp"


class ProjectCompilationInput final : public CompilationInput
{
public:
    explicit ProjectCompilationInput();

    std::filesystem::path inputFilePath() override;
    std::filesystem::path outputFilePath() override;
    std::vector<std::string> compilationDefines() override;

protected:
    void validateSourceFiles();

    std::string buildInputFile() override;
    std::vector<std::filesystem::path> inputDependencies() override;

    bool use_cpp_deserialize_input_;
    bool use_cpp_serialize_output_;
};


#endif //ALGATORC_PROJECTCOMPILATIONINPUT_HPP