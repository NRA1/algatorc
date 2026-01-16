#ifndef ALGATORC_PROJECT_HPP
#define ALGATORC_PROJECT_HPP

void __report_error(const char* error);


#ifdef ALGATORC
struct input;
struct output;
struct input* deserialize_input(char* bytes, unsigned int n);
char* serialize_output(struct output* output, unsigned int* n);

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
struct input* __deserialize_input(char* bytes, unsigned int n)
{
    return deserialize_input(bytes, n);
}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
char* __serialize_output(struct output* output, unsigned int* n)
{
    return serialize_output(output, n);
}



#endif

#ifdef ALGATORCPP
#include <exception>

class input;
class output;
input* deserialize_input(char* bytes, unsigned int n);
char* serialize_output(output* output, unsigned int* n);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    struct input* __deserialize_input(char* bytes, const unsigned int n)
    {
        try
        {
            return deserialize_input(bytes, n);
        }
        catch (std::exception& e)
        {
            __report_error(e.what());
            throw;
        }
    }
}



extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    char* __serialize_output(struct output* output, unsigned int* n)
    {
        try
        {
            return serialize_output(output, n);
        }
        catch (std::exception& e)
        {
            __report_error(e.what());
            throw;
        }
    }
}

#endif

#endif //ALGATORC_PROJECT_HPP