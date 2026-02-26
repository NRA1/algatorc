#include "ProjectLibrary.hpp"

#include "../Support/Guard.hpp"

ProjectLibrary::ProjectLibrary(void* handle) : DynamicLibrary(handle)
{
    deserialize_input_func_ = resolve<void*(*)(char*, unsigned int)>("__deserialize_input");
    serialize_output_func_ = resolve<char*(*)(void*, unsigned int*)>("__serialize_output");
}

std::variant<ProjectLibrary, std::string> ProjectLibrary::tryLoadFrom(const std::filesystem::path& path)
{
    return DynamicLibrary::tryLoadFrom<ProjectLibrary>(path);
}

void* ProjectLibrary::deserializeInput(char* bytes, const unsigned int n)
{
    return executeInContext<void*>("deserialize_input", [&]()
    {
        return deserialize_input_func_(bytes, n);
    });
}

char* ProjectLibrary::serializeOutput(void* output, unsigned int* n)
{
    return executeInContext<char*>("serialize_output", [&]()
    {
        return serialize_output_func_(output, n);
    });
}

void ProjectLibrary::swap(ProjectLibrary& other) noexcept
{
    DynamicLibrary::swap(other);
    std::swap(deserialize_input_func_, other.deserialize_input_func_);
    std::swap(serialize_output_func_, other.serialize_output_func_);
}

void swap(ProjectLibrary& first, ProjectLibrary& second) noexcept
{
    first.swap(second);
}
