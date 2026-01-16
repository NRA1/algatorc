#ifndef ALGATORC_ALGORITHM_HPP
#define ALGATORC_ALGORITHM_HPP
#include <iostream>

void __report_error(const char* error);

#ifdef ALGATORC
struct input;
struct output;

struct output* execute(struct input* input);

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
output* __execute(struct input* input)
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
            std::cout << "B1" << std::endl;
            throw std::exception();
            std::cout << "A1" << std::endl;
        } catch (...)
        {
            std::cout << "Caught" << std::endl;
        }

        try
        {
            std::cout << "Before" << std::endl;
            auto a = execute(input);
            std::cout << "After" << std::endl;
            return a;
        }
        catch (std::exception& e)
        {
            std::cout << "HIT" << std::endl;
            __report_error(e.what());
            throw;
        }
        catch (...)
        {
            std::cout << "HIT2" << std::endl;
            __report_error("Unknown exception");
            throw;
        }
    }
}
#endif

#endif //ALGATORC_ALGORITHM_HPP