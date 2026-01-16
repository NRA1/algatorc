#include "TestCi.hpp"

#include "../Support/Configuration.hpp"

TestCI::TestCI(Configuration& config) : config_(config)
{

}

std::filesystem::path TestCI::inputFilePath()
{
    return config_.temporaryDir() / "test.cpp";
}

std::filesystem::path TestCI::outputFilePath()
{
    return config_.temporaryDir() / "test.so";
}

std::string TestCI::buildInputFile()
{
    std::stringstream stream;
    stream << "#include <exception>\n";
    stream << "extern \"C\" void test() { try { throw std::exception(); } catch (...) { }; }";
    std::string s = stream.str();
    return s;
}

std::vector<std::filesystem::path> TestCI::inputDependencies()
{
    return {};
}
