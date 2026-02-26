#include "Configuration.hpp"

#include <cassert>
#include <climits>
#include <cstring>

#include "Error.hpp"
#include <vector>

std::optional<Configuration> Configuration::configuration_ = std::nullopt;

void Configuration::parse(int argc, char* argv[])
{
    if (argc < 5)
        error(ErrorType::System, "Invalid number of arguments");

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
        error(ErrorType::System) << "Invalid <times_to_execute> value: '" << argv[4] << "'. Expected an integer.";
    }
    catch (std::out_of_range&)
    {
        error(ErrorType::System, "<times_to_execute> too large");
    }


    std::optional<std::filesystem::path> data_root_path_opt = std::nullopt;
    std::optional<std::filesystem::path> data_local_path_opt = std::nullopt;
    bool force_recompile = false;
    bool deserialize_each_execute = false;

    for (int i = 5; i < argc; i++)
    {
        if (strcmp(argv[i], "-dr") == 0)
        {
            if (argc <= i + 1)
                error(ErrorType::System) << "Missing path after '-dr'";
            data_root_path_opt = std::filesystem::path(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-dl") == 0)
        {
            if (argc <= i + 1)
                error(ErrorType::System) << "Missing path after '-dl'";

            data_local_path_opt = std::filesystem::path(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            force_recompile = true;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            deserialize_each_execute = true;
        }
        else
        {
            error(ErrorType::System) << "Invalid argument: " << argv[i];
        }

    }

    std::filesystem::path data_root_path;
    std::filesystem::path data_local_path;
    if (!data_root_path_opt.has_value() || !data_local_path_opt.has_value())
    {
        const char* path = std::getenv("ALGATOR_ROOT");
        if (path == nullptr || path[0] == '\0')
        {
            error(ErrorType::System, "Either ALGATOR_ROOT environment variable must be set or flags"
                                     " '-dr <algator_data_root_path>' and '-dl <algator_data_local_path>' must"
                                     " be provided.");
        }

        std::filesystem::path algator_root_path;
        try
        {
            algator_root_path = std::filesystem::path(path);
        }
        catch (std::exception&)
        {
            error(ErrorType::System) << "Invalid ALGATOR_ROOT path: '" << path << "'";
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

    std::filesystem::path input_file_path = io_filename_root.string() + ".input";
    if (!std::filesystem::exists(input_file_path))
        error(ErrorType::System) << "Input file " << input_file_path.string() << " does not exist.";

    std::filesystem::path project_dir = (data_root_path / "projects" / "PROJ-").concat(project_name);
    if (!std::filesystem::exists(project_dir))
        error(ErrorType::System) << "Project directory " << project_dir.string() << " does not exist.";

    std::filesystem::path input_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "input" SOURCE_EXTENSION;
    if (!std::filesystem::exists(input_src_file_path))
        error(ErrorType::System) << "Input source file " << input_src_file_path.string() << " does not exist.";

    std::filesystem::path output_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "output" SOURCE_EXTENSION;
    if (!std::filesystem::exists(output_src_file_path))
        error(ErrorType::System) << "Output source file " << output_src_file_path.string() << " does not exist.";

    std::filesystem::path data_converter_src_file_path = project_dir / "proj" / PROJECT_SOURCE_DIR_NAME / "data_converter" SOURCE_EXTENSION;
    if (!std::filesystem::exists(data_converter_src_file_path))
        error(ErrorType::System) << "Data converter source file " << data_converter_src_file_path.string() << " does not exist.";

    std::filesystem::path algorithm_src_file_path = project_dir / "algs" / (std::string("ALG-") + algorithm_name) / "src" / "algorithm" SOURCE_EXTENSION;
    if (!std::filesystem::exists(algorithm_src_file_path))
        error(ErrorType::System) << "Algorithm source file " << algorithm_src_file_path.string() << " does not exist.";

    std::filesystem::path output_file_path = io_filename_root.string() + ".output";
    std::filesystem::path status_file_path = io_filename_root.string() + ".status";

    std::filesystem::path temporary_dir = data_local_path / "tmp" / project_name;

    std::filesystem::path data_local_project_dir = (data_local_path / "projects" / "PROJ-").concat(project_name);
    std::filesystem::path algorithm_bin_dir = (data_local_project_dir / "algs" / "ALG-").concat(algorithm_name) / "bin";
    std::filesystem::path project_bin_dir = data_local_project_dir / "proj" / "bin";

    configuration_ = {times_to_execute, input_file_path, output_file_path, status_file_path, input_src_file_path,
        output_src_file_path, data_converter_src_file_path, algorithm_src_file_path, temporary_dir,
        algorithm_bin_dir, project_bin_dir, force_recompile, deserialize_each_execute};
}

bool Configuration::initialized()
{
    return configuration_.has_value();
}

Configuration& Configuration::get()
{
    assert(configuration_.has_value() && "Configuration must be set before first query");
    return configuration_.value();
}

Configuration::Configuration(const unsigned int times_to_execute, const std::filesystem::path& input_file_path,
                             const std::filesystem::path& output_file_path, const std::filesystem::path& status_file_path,
                             const std::filesystem::path& input_src_file_path, const std::filesystem::path& output_src_file_path,
                             const std::filesystem::path& data_converter_src_file_path, const std::filesystem::path& algorithm_src_file_path,
                             const std::filesystem::path& temporary_dir, const std::filesystem::path& algorithm_bin_dir,
                             const std::filesystem::path& project_bin_dir, const bool force_recompile, const bool deserialize_each_execute)
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
    force_recompile_ = force_recompile;
    deserialize_each_execute_ = deserialize_each_execute;
}

unsigned int Configuration::timesToExecute()
{
    return get().times_to_execute_;
}

const std::filesystem::path& Configuration::inputFilePath()
{
    return get().input_file_path_;
}

const std::filesystem::path& Configuration::outputFilePath()
{
    return get().output_file_path_;
}

const std::filesystem::path& Configuration::statusFilePath()
{
    return get().status_file_path_;
}

const std::filesystem::path& Configuration::inputSrcFilePath()
{
    return get().input_src_file_path_;
}

const std::filesystem::path& Configuration::outputSrcFilePath()
{
    return get().output_src_file_path_;
}

const std::filesystem::path& Configuration::dataConverterSrcFilePath()
{
    return get().data_converter_src_file_path_;
}

const std::filesystem::path& Configuration::algorithmSrcFilePath()
{
    return get().algorithm_src_file_path_;
}

const std::filesystem::path& Configuration::temporaryDir()
{
    return get().temporary_dir_;
}

const std::filesystem::path& Configuration::algorithmBinDir()
{
    return get().algorithm_bin_dir_;
}

const std::filesystem::path& Configuration::projectBinDir()
{
    return get().project_bin_dir_;
}

bool Configuration::forceRecompile()
{
    return get().force_recompile_;
}

bool Configuration::deserializeEachExecute()
{
    return get().deserialize_each_execute_;
}
