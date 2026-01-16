#ifndef ALGATORC_ALGORITHM_HPP
#define ALGATORC_ALGORITHM_HPP

void __report_error(const char* error);

#ifdef ALGATORC
struct input;
struct output;

struct output* execute(struct input* input);

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
struct output* __execute(struct input* input)
{
    return execute(input);
}

#endif

#ifdef ALGATORCPP
#include <exception>

class input;
class output;

output* execute(input* input);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    output* __execute(input* input)
    {
        try
        {
            return execute(input);
        }
        catch (std::exception& e)
        {
            __report_error(e.what());
            return nullptr;
        }
        catch (...)
        {
            __report_error("Unknown exception");
            return nullptr;
        }
    }
}
#endif

#endif //ALGATORC_ALGORITHM_HPP