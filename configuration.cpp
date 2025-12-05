#include "configuration.hpp"

#include <assert.h>
#include <climits>
#include <cstring>

#include "error.hpp"

configuration configuration::parse(int argc, char* argv[])
{
    if (argc < 5 || argc == 6 || argc == 8 || argc > 9)
        error(error::System, error::Preparation) << "Invalid number of arguments";

    std::optional<std::filesystem::path> data_root_path_opt = std::nullopt;
    std::optional<std::filesystem::path> data_local_path_opt = std::nullopt;

    if (argc == 7 || argc == 9)
        parse_flags(argv[5], argv[6], data_root_path_opt, data_local_path_opt);

    if (argc == 9)
        parse_flags(argv[7], argv[8], data_root_path_opt, data_local_path_opt);

    const std::string project_name = std::string(argv[1]);
    const std::string algorithm_name = std::string(argv[2]);

    std::filesystem::path io_filename_root = std::filesystem::path(argv[3]);

    unsigned int times_to_execute;
    try
    {
        unsigned long tte_long = std::stoul(std::string(argv[4]));
        if (tte_long > INT_MAX) throw std::out_of_range("Out of range");
        times_to_execute = tte_long;
    }
    catch (std::invalid_argument&)
    {
        error(error::System, error::Preparation) << "Invalid <times_to_execute> value: '" << argv[4] << "'. Expected an integer.";
    }
    catch (std::out_of_range&)
    {
        error(error::System, error::Preparation) << "<times_to_execute> too large";
    }


    std::filesystem::path data_root_path;
    std::filesystem::path data_local_path;
    if (!data_root_path_opt.has_value() || !data_local_path_opt.has_value())
    {
        const char* path = std::getenv("ALGATOR_ROOT");
        if (path == nullptr || path[0] == '\0')
        {
            error(
                error::System,
                error::Preparation,
                "Either ALGATOR_ROOT environment variable must be set or flags '-dr <algator_data_root_path>'"
                " and '-dl <algator_data_local_path>' must be provided.");
        }

        std::filesystem::path algator_root_path;
        try
        {
            algator_root_path = std::filesystem::path(path);
        }
        catch (std::exception&)
        {
            error(error::System, error::Preparation) << "Invalid ALGATOR_ROOT path: '" << path << "'";
        }

        data_root_path = algator_root_path / std::filesystem::path("data_root");
        data_local_path = algator_root_path / std::filesystem::path("data_local");
    }
    else
    {
        data_root_path = data_root_path_opt.value();
        data_local_path = data_local_path_opt.value();
    }

    std::filesystem::path cwd = std::filesystem::current_path();
    if (io_filename_root.is_relative())
    {
        io_filename_root = cwd / io_filename_root;
    }
    if (data_root_path.is_relative())
    {
        data_root_path = cwd / data_root_path;
    }
    if (data_local_path_opt->is_relative())
    {
        data_local_path = cwd / data_local_path;
    }

    std::filesystem::path input_file_path = io_filename_root.concat(".input");
    if (!std::filesystem::exists(input_file_path))
        error(error::System, error::Preparation) << "Input file " << input_file_path << " does not exist.";

    std::filesystem::path project_dir = (data_root_path / "projects" / "PROJ").concat(project_name);
    if (!std::filesystem::exists(project_dir))
        error(error::System, error::Preparation) << "Project directory " << project_dir << " does not exist.";

    std::filesystem::path input_src_file_path = project_dir / PROJECT_SOURCE_DIR_NAME / "input" SOURCE_EXTENSION;
    if (!std::filesystem::exists(input_src_file_path))
        error(error::System, error::Preparation) << "Input source file " << input_src_file_path << " does not exist.";

    std::filesystem::path output_src_file_path = project_dir / PROJECT_SOURCE_DIR_NAME / "output" SOURCE_EXTENSION;
    if (!std::filesystem::exists(output_src_file_path))
        error(error::System, error::Preparation) << "Output source file " << output_src_file_path << " does not exist.";

    std::filesystem::path data_converter_src_file_path = project_dir / PROJECT_SOURCE_DIR_NAME / "data_converter" SOURCE_EXTENSION;
    if (!std::filesystem::exists(data_converter_src_file_path))
        error(error::System, error::Preparation) << "Data converter source file " << data_converter_src_file_path << " does not exist.";

    std::filesystem::path algorithm_src_file_path = project_dir / "algs" / (std::string("ALG-") + algorithm_name) / "src" / "algorithm" SOURCE_EXTENSION;
    if (!std::filesystem::exists(algorithm_src_file_path))
        error(error::System, error::Preparation) << "Algorithm source file " << algorithm_src_file_path << " does not exist.";

    std::filesystem::path output_file_path = io_filename_root.concat(".output");
    std::filesystem::path status_file_path = io_filename_root.concat(".status");

    return configuration(times_to_execute, input_file_path, output_file_path, status_file_path, input_src_file_path,
        output_src_file_path, data_converter_src_file_path, algorithm_src_file_path, data_local_path);
}

void configuration::parse_flags(const char* flag, const char* path,
    std::optional<std::filesystem::path>& data_root_path, std::optional<std::filesystem::path>& data_local_path)
{
    if (strcmp(flag, "-dr") == 0)
    {
        data_root_path = std::filesystem::path(path);
    }
    else if (strcmp(flag, "-dl") == 0)
    {
        data_local_path = std::filesystem::path(path);
    }
    else
    {
        error(error::System, error::Preparation) << "Invalid argument: " << flag;
    }
}

configuration::configuration(const unsigned int times_to_execute, const std::filesystem::path& input_file_path,
    const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
    const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
    const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
    const std::filesystem::path& data_local_path)
{
    m_times_to_execute = times_to_execute;
    m_input_file_path = input_file_path;
    m_output_file_path = output_file_path,
    m_status_file_path = status_file_path;
    m_input_src_file_path = input_src_file_path;
    m_output_src_file_path = output_src_file_path;
    m_data_converter_src_file_path = data_converter_src_file_path;
    m_algorithm_src_file_path = algorithm_src_file_path;
    m_data_local_path = data_local_path;
}