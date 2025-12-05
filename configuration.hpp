#ifndef ALGATORC_CONFIGURATION_HPP
#define ALGATORC_CONFIGURATION_HPP
#include <filesystem>

#ifdef ALGATORC

#define SOURCE_DIR_NAME "c-src"
#define SOURCE_EXTENSION ".c"

#endif
#ifdef ALGATORCPP

#define PROJECT_SOURCE_DIR_NAME "cpp-src"
#define SOURCE_EXTENSION ".cpp"

#endif

class configuration
{
public:
    static configuration parse(int argc, char* argv[]);

private:
    static void parse_flags(const char* flag, const char* path, std::optional<std::filesystem::path>& data_root_path,
                 std::optional<std::filesystem::path>& data_local_path);

    configuration(unsigned int times_to_execute, const std::filesystem::path& input_file_path,
        const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
        const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
        const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
        const std::filesystem::path& data_local_path);

    unsigned int m_times_to_execute;
    std::filesystem::path m_input_file_path;
    std::filesystem::path m_output_file_path;
    std::filesystem::path m_status_file_path;
    std::filesystem::path m_input_src_file_path;
    std::filesystem::path m_output_src_file_path;
    std::filesystem::path m_data_converter_src_file_path;
    std::filesystem::path m_algorithm_src_file_path;

    std::filesystem::path m_data_local_path; //TODO: make more specific?
};


#endif //ALGATORC_CONFIGURATION_HPP