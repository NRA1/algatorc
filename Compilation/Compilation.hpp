#ifndef ALGATORC_COMPILATION_HPP
#define ALGATORC_COMPILATION_HPP
#include <utility>

#include "Compiler.hpp"
#include "../Support/Configuration.hpp"

template<typename TInput, typename TLibrary>
TLibrary compileAndLoad(Compiler& compiler)
{
    Error::setPhase(ErrorPhase::Compilation);

    TInput input;

    const bool recompile = Configuration::forceRecompile() || input.compilationNeeded();
    if (recompile)
    {
        compiler.compile(input);

        input.clean();
    }

    Error::setPhase(ErrorPhase::Setup);

    std::variant<TLibrary, std::string> variant = TLibrary::tryLoadFrom(input.outputFilePath());

    if (std::holds_alternative<std::string>(variant) && !recompile)
    {
        Error::setPhase(ErrorPhase::Compilation);

        compiler.compile(input);
        input.clean();

        std::variant<TLibrary, std::string> new_variant = TLibrary::tryLoadFrom(input.outputFilePath());
        variant.swap(new_variant);

        Error::setPhase(ErrorPhase::Setup);
    }

    if (const std::string* err = std::get_if<std::string>(&variant))
    {
        error(ErrorType::System, "Failed to load project dynamic library") << *err;
    }

    TLibrary library = std::move(std::get<TLibrary>(variant));
    return library;
}

#endif //ALGATORC_COMPILATION_HPP
