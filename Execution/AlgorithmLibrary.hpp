#ifndef ALGATORC_ALGORITHMLIBRARY_HPP
#define ALGATORC_ALGORITHMLIBRARY_HPP
#include "DynamicLibrary.hpp"


class AlgorithmLibrary : public DynamicLibrary
{
public:
    explicit AlgorithmLibrary(const std::filesystem::path& path);

    void* execute(void* input);

private:
    void* (*execute_func_)(void*);
};

#endif //ALGATORC_ALGORITHMLIBRARY_HPP