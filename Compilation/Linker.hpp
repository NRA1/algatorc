#ifndef ALGATORC_LINKER_HPP
#define ALGATORC_LINKER_HPP

#include <lld/Common/Driver.h>

#include "CompilationInput.hpp"

LLD_HAS_DRIVER(elf)

class Linker
{
public:
    Linker();

    void link(CompilationInput& input);

private:
    static std::vector<std::string> findLibraryPaths();

    std::vector<std::string> default_args_;
};


#endif //ALGATORC_LINKER_HPP