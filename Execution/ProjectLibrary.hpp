#ifndef ALGATORC_PROJECTLIBRARY_HPP
#define ALGATORC_PROJECTLIBRARY_HPP
#include "DynamicLibrary.hpp"


class ProjectLibrary : public DynamicLibrary
{
public:
    explicit ProjectLibrary(const std::filesystem::path& path);

    void* deserializeInput(char* bytes, unsigned int n) const;
    char* serializeOutput(void* output, unsigned int* n) const;

private:
    void* (*deserialize_input_func_)(char*, unsigned int);
    char* (*serialize_output_func_)(void*, unsigned int*);
};


#endif //ALGATORC_PROJECTLIBRARY_HPP