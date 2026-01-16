#include "Configuration.hpp"

#include <cassert>
#include <climits>
#include <cstring>

#include "Error.hpp"

Configuration Configuration::parse(int argc, char* argv[])
{
    if (argc < 5 || argc == 6 || argc == 8 || argc > 9)
        error(ErrorType::System, ErrorPhase::Preparation) << "Invalid number of arguments";

    std::optional<std::filesystem::path> data_root_path_opt = std::nullopt;
    std::optional<std::filesystem::path> data_local_path_opt = std::nullopt;

    if (argc == 7 || argc == 9)
        parseFlags(argv[5], argv[6], data_root_path_opt, data_local_path_opt);

    if (argc == 9)
        parseFlags(argv[7], argv[8], data_root_path_opt, data_local_path_opt);

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
        error(ErrorType::System, ErrorPhase::Preparation) << "Invalid <times_to_execute> value: '" << argv[4] << "'. Expected an integer.";
    }
    catch (std::out_of_range&)
    {
        error(ErrorType::System, ErrorPhase::Preparation) << "<times_to_execute> too large";
    }


    std::filesystem::path data_root_path;
    std::filesystem::path data_local_path;
    if (!data_root_path_opt.has_value() || !data_local_path_opt.has_value())
    {
        const char* path = std::getenv("ALGATOR_ROOT");
        if (path == nullptr || path[0] == '\0')
        {
            error(ErrorType::System, ErrorPhase::Preparation,
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
            error(ErrorType::System, ErrorPhase::Preparation) << "Invalid ALGATOR_ROOT path: '" << path << "'";
        }

        if (!data_root_path_opt.has_value())
            data_root_path = algator_root_path / std::filesystem::path("data_root");
        if (!data_local_path_opt.has_value())
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
    if (data_local_path.is_relative())
    {
        data_local_path = cwd / data_local_path;
    }

    std::filesystem::path input_file_path = io_filename_root.concat(".input");
    if (!std::filesystem::exists(input_file_path))
        error(ErrorType::System, ErrorPhase::Preparation) << "Input file " << input_file_path << " does not exist.";

    std::filesystem::path project_dir = (data_root_path / "projects" / "PROJ-").concat(project_name);
    if (!std::filesystem::exists(project_dir))
        error(ErrorType::System, ErrorPhase::Preparation) << "Project directory " << project_dir << " does not exist.";

    std::filesystem::path input_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "input" SOURCE_EXTENSION;
    if (!std::filesystem::exists(input_src_file_path))
        error(ErrorType::System, ErrorPhase::Preparation) << "Input source file " << input_src_file_path << " does not exist.";

    std::filesystem::path output_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "output" SOURCE_EXTENSION;
    if (!std::filesystem::exists(output_src_file_path))
        error(ErrorType::System, ErrorPhase::Preparation) << "Output source file " << output_src_file_path << " does not exist.";

    std::filesystem::path data_converter_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "data_converter" SOURCE_EXTENSION;
    if (!std::filesystem::exists(data_converter_src_file_path))
        error(ErrorType::System, ErrorPhase::Preparation) << "Data converter source file " << data_converter_src_file_path << " does not exist.";

    std::filesystem::path algorithm_src_file_path = project_dir / "algs" / (std::string("ALG-") + algorithm_name) / "src" / "algorithm" SOURCE_EXTENSION;
    if (!std::filesystem::exists(algorithm_src_file_path))
        error(ErrorType::System, ErrorPhase::Preparation) << "Algorithm source file " << algorithm_src_file_path << " does not exist.";

    std::filesystem::path output_file_path = io_filename_root.concat(".output");
    std::filesystem::path status_file_path = io_filename_root.concat(".status");

    std::filesystem::path temporary_dir = data_local_path / "tmp" / project_name;

    std::filesystem::path data_local_project_dir = (data_local_path / "projects" / "PROJ-").concat(project_name);
    std::filesystem::path algorithm_bin_dir = (data_local_project_dir / "algs" / "ALG-").concat(algorithm_name) / "bin";
    std::filesystem::path project_bin_dir = data_local_project_dir / "proj" / "bin";

    return {times_to_execute, input_file_path, output_file_path, status_file_path, input_src_file_path,
        output_src_file_path, data_converter_src_file_path, algorithm_src_file_path, temporary_dir,
        algorithm_bin_dir, project_bin_dir};
}

void Configuration::parseFlags(const char* flag, const char* path,
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
        error(ErrorType::System, ErrorPhase::Preparation) << "Invalid argument: " << flag;
    }
}

Configuration::Configuration(const unsigned int times_to_execute, const std::filesystem::path& input_file_path,
    const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
    const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
    const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
    const std::filesystem::path& temporary_dir, const std::filesystem::path& algorithm_bin_dir,
    const std::filesystem::path& project_bin_dir)
{
    times_to_execute_ = times_to_execute;
    input_file_path_ = input_file_path;
    output_file_path_ = output_file_path,
    status_file_path_ = status_file_path;
    input_src_file_path_ = input_src_file_path;
    output_src_file_path_ = output_src_file_path;
    data_converter_src_file_path_ = data_converter_src_file_path;
    algorithm_src_file_path_ = algorithm_src_file_path;
    temporary_dir_ = temporary_dir;
    algorithm_bin_dir_ = algorithm_bin_dir;
    project_bin_dir_ = project_bin_dir;
}

unsigned int Configuration::timesToExecute() const
{
    return times_to_execute_;
}

const std::filesystem::path& Configuration::inputFilePath() const
{
    return input_file_path_;
}

const std::filesystem::path& Configuration::outputFilePath() const
{
    return output_file_path_;
}

const std::filesystem::path& Configuration::statusFilePath() const
{
    return status_file_path_;
}

const std::filesystem::path& Configuration::inputSrcFilePath() const
{
    return input_src_file_path_;
}

const std::filesystem::path& Configuration::outputSrcFilePath() const
{
    return output_src_file_path_;
}

const std::filesystem::path& Configuration::dataConverterSrcFilePath() const
{
    return data_converter_src_file_path_;
}

const std::filesystem::path& Configuration::algorithmSrcFilePath() const
{
    return algorithm_src_file_path_;
}

const std::filesystem::path& Configuration::temporaryDir() const
{
    return temporary_dir_;
}

const std::filesystem::path& Configuration::algorithmBinDir() const
{
    return algorithm_bin_dir_;
}

const std::filesystem::path& Configuration::projectBinDir() const
{
    return project_bin_dir_;
}
