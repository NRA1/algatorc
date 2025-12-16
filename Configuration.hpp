#ifndef ALGATORC_CONFIGURATION_HPP
#define ALGATORC_CONFIGURATION_HPP
#include <filesystem>
#include <ostream>

#ifdef ALGATORC

#define PROJECT_SOURCE_DIR_NAME "c-src"
#define SOURCE_EXTENSION ".c"

#endif
#ifdef ALGATORCPP

#define PROJECT_SOURCE_DIR_NAME "cpp-src"
#define SOURCE_EXTENSION ".cpp"

#endif

class Configuration
{
public:
    static Configuration parse(int argc, char* argv[]);

    [[nodiscard]] unsigned int timesToExecute() const;
    [[nodiscard]] const std::filesystem::path& inputFilePath() const;
    [[nodiscard]] const std::filesystem::path& outputFilePath() const;
    [[nodiscard]] const std::filesystem::path& statusFilePath() const;
    [[nodiscard]] const std::filesystem::path& inputSrcFilePath() const;
    [[nodiscard]] const std::filesystem::path& outputSrcFilePath() const;
    [[nodiscard]] const std::filesystem::path& dataConverterSrcFilePath() const;
    [[nodiscard]] const std::filesystem::path& algorithmSrcFilePath() const;
    [[nodiscard]] const std::filesystem::path& temporaryDir() const;
    [[nodiscard]] const std::filesystem::path& algorithmBinDir() const;
    [[nodiscard]] const std::filesystem::path& projectBinDir() const;

private:
    static void parseFlags(const char* flag, const char* path, std::optional<std::filesystem::path>& data_root_path,
                 std::optional<std::filesystem::path>& data_local_path);

    Configuration(unsigned int times_to_execute, const std::filesystem::path& input_file_path,
        const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
        const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
        const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
        const std::filesystem::path& temporary_dir, const std::filesystem::path& algorithm_bin_dir,
        const std::filesystem::path& project_bin_dir);

    unsigned int times_to_execute_;
    std::filesystem::path input_file_path_;

    std::filesystem::path output_file_path_;
    std::filesystem::path status_file_path_;
    std::filesystem::path input_src_file_path_;
    std::filesystem::path output_src_file_path_;
    std::filesystem::path data_converter_src_file_path_;
    std::filesystem::path algorithm_src_file_path_;

    std::filesystem::path temporary_dir_;
    std::filesystem::path algorithm_bin_dir_;
    std::filesystem::path project_bin_dir_;
};


#endif //ALGATORC_CONFIGURATION_HPP