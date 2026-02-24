#include "../CompilationInput/ProjectCompilationInput.hpp"

#include <iostream>

#include "../../Support/Error.hpp"
#include <Templates/Project.hpp>
#include <Templates/ErrorReporting.hpp>
#include <Templates/InputByteBuf.hpp>
#include <Templates/OutputByteBuf.hpp>

#include "../TranslationUnit.hpp"
#include "../FunctionValidator/CDeserializeInputValidator.hpp"
#include "../FunctionValidator/CppDeserializeInputValidator.hpp"
#include "../FunctionValidator/CppSerializeOutputValidator.hpp"
#include "../FunctionValidator/CSerializeOutputValidator.hpp"
#include "../TranslationInput/DataConverterTranslationInput.hpp"

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

    DataConverterTranslationInput data_converter_input{};
    const TranslationUnit unit{data_converter_input};

    const bool c_deserialize_input = unit.contains(CDeserializeInputValidator{});
#ifdef ALGATORCPP
    const bool cpp_deserialize_input = unit.contains(CppDeserializeInputValidator{});
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
                               " \"input* deserialize_input(char*, unsigned int)\""
                               " or format \"input* deserialize_input(std:istream&)\".");
#else
        error(ErrorType::User, "\"deserialize_input\" is not defined. Define a deserialization function with format"
                       " \"input* deserialize_input(char*, const unsigned int)\".");
#endif
    }

    //TODO: validate other
    const bool c_serialize_output = unit.contains(CSerializeOutputValidator{});
#ifdef ALGATORCPP
    const bool cpp_serialize_output = unit.contains(CppSerializeOutputValidator{});
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
                               " or format \"void serialize_output(std:ostream&, output*)\".");
#else
        error(ErrorType::User, "\"serialize_output\" is not defined. Define a serialization function with format"
                       " \"char* serialize_output(output*, unsigned int*)\".");
#endif
    }
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
    if (use_cpp_deserialize_input_) defines.push_back("CPP_DESERIALIZE_INPUT");
    if (use_cpp_serialize_output_) defines.push_back("CPP_SERIALIZE_OUTPUT");
    return defines;
}

ProjectLibrary ProjectCompilationInput::loadDynamicLibrary()
{
    return CompilationInput::loadDynamicLibrary<ProjectLibrary>();
}

std::string ProjectCompilationInput::buildInputFile()
{
    std::stringstream stream;
    stream << ErrorReportingTemplate << "\n";
    stream << "#include " << Configuration::inputSrcFilePath() << "\n";
    stream << "#include " << Configuration::outputSrcFilePath() << "\n";
    stream << "#include " << Configuration::dataConverterSrcFilePath() << "\n";
    if (use_cpp_deserialize_input_) stream << InputByteBufTemplate << "\n";
    if (use_cpp_serialize_output_) stream << OutputByteBufTemplate << "\n";
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