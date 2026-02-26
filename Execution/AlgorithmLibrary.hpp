#ifndef ALGATORC_ALGORITHMLIBRARY_HPP
#define ALGATORC_ALGORITHMLIBRARY_HPP
#include "DynamicLibrary.hpp"


class AlgorithmLibrary : public DynamicLibrary
{
    friend class DynamicLibrary;
public:
    static std::variant<AlgorithmLibrary, std::string> tryLoadFrom(const std::filesystem::path& path);

    void* execute(void* input);

private:
    explicit AlgorithmLibrary(void* handle);

    friend void swap(AlgorithmLibrary& first, AlgorithmLibrary& second) noexcept;
    void swap(AlgorithmLibrary& other) noexcept;

    void* (*execute_func_)(void*);
};

#endif //ALGATORC_ALGORITHMLIBRARY_HPP