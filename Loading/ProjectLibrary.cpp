#include "ProjectLibrary.hpp"

#include "../Support/Guard.hpp"

ProjectLibrary::ProjectLibrary(const std::filesystem::path& path) : DynamicLibrary(path)
{
    deserialize_input_func_ = resolve<void*(*)(char*, unsigned int)>("__deserialize_input");
    serialize_output_func_ = resolve<char*(*)(void*, unsigned int*)>("__serialize_output");
}

void* ProjectLibrary::deserializeInput(char* bytes, const unsigned int n) const
{
    return guard<void*>("deserialize_input", error_func_, clear_error_func_, [&]()
    {
        return deserialize_input_func_(bytes, n);
    });
}

char* ProjectLibrary::serializeOutput(void* output, unsigned int* n) const
{
    return guard<char*>("serialize_output", error_func_, clear_error_func_, [&]()
    {
        return serialize_output_func_(output, n);
    });
}
