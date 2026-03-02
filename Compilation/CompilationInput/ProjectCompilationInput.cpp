#include "../CompilationInput/ProjectCompilationInput.hpp"

#include <iostream>

#include "../../Support/Error.hpp"
#include <Templates/Project.hpp>
#include <Templates/ErrorReporting.hpp>
#include <Templates/InputByteBuf.hpp>
#include <Templates/OutputByteBuf.hpp>

#include "../TranslationUnit.hpp"
#include "../CodeValidator/CDeserializeInputValidator.hpp"
#include "../CodeValidator/CppDeserializeInputValidator.hpp"
#include "../CodeValidator/CppSerializeOutputValidator.hpp"
#include "../CodeValidator/CSerializeOutputValidator.hpp"
#include "../CodeValidator/InputDeepCopyValidator.hpp"
#include "../CodeValidator/InputValidator.hpp"
#include "../CodeValidator/OutputValidator.hpp"
#include "../TranslationInput/DataConverterTranslationInput.hpp"
#include "../TranslationInput/InputTranslationInput.hpp"
#include "../TranslationInput/OutputTranslationInput.hpp"

ProjectCompilationInput::ProjectCompilationInput()
{
    try
    {
        if (!std::filesystem::exists(Configuration::temporaryDir()))
            std::filesystem::create_directories(Configuration::temporaryDir());

        if (!std::filesystem::exists(Configuration::projectBinDir()))
            std::filesystem::create_directories(Configuration::projectBinDir());
    }
    catch (std::filesystem::filesystem_error& e)
    {
        error(ErrorType::System, "Failed to create binary directories: ") << e.what();
    }

    validateSourceFiles();
}

std::filesystem::path ProjectCompilationInput::inputFilePath()
{
    return Configuration::temporaryDir() / "project" SOURCE_EXTENSION;
}

std::filesystem::path ProjectCompilationInput::outputFilePath()
{
    return Configuration::projectBinDir() / "project" SOURCE_EXTENSION DYLIB_EXTENSION;
}

std::vector<std::string> ProjectCompilationInput::compilationDefines()
{
    std::vector<std::string> defines;
    if (use_cpp_deserialize_input_) defines.emplace_back("CPP_DESERIALIZE_INPUT");
    if (use_cpp_serialize_output_) defines.emplace_back("CPP_SERIALIZE_OUTPUT");
    return defines;
}

void ProjectCompilationInput::validateSourceFiles()
{
    InputTranslationInput input_translation_input{};
    const TranslationUnit input_unit{input_translation_input};
    if (!input_unit.contains(InputValidator{}))
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "\"input\" structure not defined. Define algorithm input (either a class or"
                               " a struct) with name \"input\" in \"input" SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "\"input\" structure not defined. Define algorithm input (a struct) with name \"input\""
                               " in \"input" SOURCE_EXTENSION "\".");
#endif
    }

    if (!input_unit.contains(InputDeepCopyValidator{}))
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "Input copy function not defined. Define a function that makes a deep copy of \"input\" with format"
                               " \"input* input_deep_copy(const input*)\" in \"input" SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "Input copy function not defined. Define a function that makes a deep copy of \"input\" with format"
                               " \"struct input* input_deep_copy(const struct input*)\" in \"input" SOURCE_EXTENSION "\".");
#endif
    }

    OutputTranslationInput output_translation_input{};
    const TranslationUnit output_unit{output_translation_input};
    if (!output_unit.contains(OutputValidator{}))
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "\"output\" structure not defined. Define algorithm output (either a class or"
                               " a struct) with name \"output\" in \"output" SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "\"output\" structure not defined. Define algorithm output (a struct) with name "
                               "\"output\" in \"output" SOURCE_EXTENSION "\".");
#endif
    }

    DataConverterTranslationInput data_converter_input{};
    const TranslationUnit data_converter_unit{data_converter_input};

    const bool c_deserialize_input = data_converter_unit.contains(CDeserializeInputValidator{});
#ifdef ALGATORCPP
    const bool cpp_deserialize_input = data_converter_unit.contains(CppDeserializeInputValidator{});
#else
    const bool cpp_deserialize_input = false;
#endif
    if (c_deserialize_input && cpp_deserialize_input)
    {
        std::cout << "Found both C and C++ versions of \"deserialize_input\". Will use C version.";
        use_cpp_deserialize_input_ = false;
    }
    else if (cpp_deserialize_input) use_cpp_deserialize_input_ = true;
    else if (c_deserialize_input) use_cpp_deserialize_input_ = false;
    else
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "Neither C nor C++ version of \"deserialize_input\" is defined. Define a"
                               " deserialization function either with format"
                               " \"input* deserialize_input(const char*, unsigned int)\""
                               " or format \"input* deserialize_input(std:istream&)\" in \"data_converter"
                               SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "\"deserialize_input\" is not defined. Define a deserialization function with format"
                                " \"struct input* deserialize_input(const char*, const unsigned int)\" in \"data_converter"
                                SOURCE_EXTENSION "\".");
#endif
    }

    const bool c_serialize_output = data_converter_unit.contains(CSerializeOutputValidator{});
#ifdef ALGATORCPP
    const bool cpp_serialize_output = data_converter_unit.contains(CppSerializeOutputValidator{});
#else
    const bool cpp_serialize_output = false;
#endif
    if (c_serialize_output && cpp_serialize_output)
    {
        std::cout << "Found both C and C++ versions of \"serialize_output\". Will use C version.";
        use_cpp_serialize_output_ = false;
    }
    else if (cpp_serialize_output) use_cpp_serialize_output_ = true;
    else if (c_serialize_output) use_cpp_serialize_output_ = false;
    else
    {
#ifdef ALGATORCPP
        error(ErrorType::User, "Neither C nor C++ version of \"serialize_output\" is defined. Define a"
                               " serialization function either with format"
                               " \"char* serialize_output(output*, unsigned int*)\""
                               " or format \"void serialize_output(std:ostream&, output*)\" in \"data_converter"
                               SOURCE_EXTENSION "\".");
#else
        error(ErrorType::User, "\"serialize_output\" is not defined. Define a serialization function with format"
                                " \"char* serialize_output(struct output*, unsigned int*)\" in \"data_converter"
                                SOURCE_EXTENSION "\".");
#endif
    }
}

std::string ProjectCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << ErrorReportingTemplate << "\n";
    if (use_cpp_deserialize_input_)
    {
        stream << "#include <istream>\n";
        stream << InputByteBufTemplate << "\n";
    }
    if (use_cpp_serialize_output_)
    {
        stream << "#include <ostream>\n";
        stream << OutputByteBufTemplate << "\n";
    }
    stream << "#include " << Configuration::inputSrcFilePath() << "\n";
    stream << "#include " << Configuration::outputSrcFilePath() << "\n";
    stream << "#include " << Configuration::dataConverterSrcFilePath() << "\n";
    stream << ProjectTemplate << "\n";
    return stream.str();
}

std::vector<std::filesystem::path> ProjectCompilationInput::inputDependencies()
{
    std::vector<std::filesystem::path> dependencies;
    dependencies.push_back(Configuration::inputSrcFilePath());
    dependencies.push_back(Configuration::outputSrcFilePath());
    dependencies.push_back(Configuration::dataConverterSrcFilePath());
    return dependencies;
}