#ifndef ALGATORC_CONFIGURATION_HPP
#define ALGATORC_CONFIGURATION_HPP
#include <filesystem>
#include <ostream>
#include <optional>

#ifdef ALGATORC

#define PROJECT_SOURCE_DIR_NAME "c-src"
#define SOURCE_EXTENSION ".c"

#endif
#ifdef ALGATORCPP

#define PROJECT_SOURCE_DIR_NAME "cpp-src"
#define SOURCE_EXTENSION ".cpp"

#endif

#ifdef __linux__

#define DYLIB_EXTENSION ".so"

#elifdef WIN32

#define DYLIB_EXTENSION ".dll"

#endif

class Configuration
{
public:
    static void parse(int argc, char* argv[]);
    static bool initialized();

    static unsigned int timesToExecute();
    static const std::filesystem::path& inputFilePath();
    static const std::filesystem::path& outputFilePath();
    static const std::filesystem::path& statusFilePath();
    static const std::filesystem::path& inputSrcFilePath();
    static const std::filesystem::path& outputSrcFilePath();
    static const std::filesystem::path& dataConverterSrcFilePath();
    static const std::filesystem::path& algorithmSrcFilePath();
    static const std::filesystem::path& temporaryDir();
    static const std::filesystem::path& algorithmBinDir();
    static const std::filesystem::path& projectBinDir();

    static bool forceRecompile();
    static bool deserializeEachExecute();

private:
    static Configuration& get();

    Configuration(unsigned int times_to_execute, const std::filesystem::path& input_file_path,
        const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
        const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
        const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
        const std::filesystem::path& temporary_dir, const std::filesystem::path& algorithm_bin_dir,
        const std::filesystem::path& project_bin_dir, bool force_recompile, bool deserialize_each_execute);

    static std::optional<Configuration> configuration_;

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

    bool force_recompile_;
    bool deserialize_each_execute_;
};


#endif //ALGATORC_CONFIGURATION_HPP