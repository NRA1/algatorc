#ifndef ALGATORC_COMPILER_HPP
#define ALGATORC_COMPILER_HPP
#include <string>
#include <vector>
#include <clang/Basic/DiagnosticOptions.h>

#include "CompilationInput.hpp"

#ifdef ALGATORC
#define CLANG_BINARY_NAME "clang"
#endif
#ifdef ALGATORCPP
#define CLANG_BINARY_NAME "clang++"
#endif

class Compiler
{
public:
    Compiler();

    void compile(CompilationInput& input);

private:
    std::vector<std::string> default_args_;
    clang::DiagnosticOptions diagnostic_options_;
};

#endif //ALGATORC_COMPILER_HPP