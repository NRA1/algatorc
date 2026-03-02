#ifndef ALGATORC_PROJECT_HPP
#define ALGATORC_PROJECT_HPP

void __report_error(const char* error);


#ifdef ALGATORC
struct input;
struct output;
struct input* input_deep_copy(const struct input*);
struct input* deserialize_input(const char* bytes, unsigned int n);
char* serialize_output(struct output* output, unsigned int* n);

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
struct input* __input_deep_copy(const struct input* input)
{
    return input_deep_copy(input);
}

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
struct input* __deserialize_input(const char* bytes, unsigned int n)
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

input* input_deep_copy(const input*);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    input* __input_deep_copy(const input* input)
    {
        try
        {
            return input_deep_copy(input);
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


#ifdef CPP_DESERIALIZE_INPUT
input* deserialize_input(std::istream&);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    struct input* __deserialize_input(const char* bytes, const unsigned int n)
    {
        InputByteBuf buf(bytes, n);
        std::istream stream(&buf);
        try
        {
            return deserialize_input(stream);
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
#else
input* deserialize_input(const char*, unsigned int);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    struct input* __deserialize_input(const char* bytes, const unsigned int n)
    {
        try
        {
            return deserialize_input(bytes, n);
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
#ifdef CPP_SERIALIZE_OUTPUT
void serialize_output(std::ostream&, output*);

extern "C" {
    // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
    char* __serialize_output(struct output* output, unsigned int* n)
    {
        OutputByteBuf buf;
        std::ostream stream(&buf);

        try
        {
            serialize_output(stream, output);
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
        return buf.takeBuffer(*n);
    }
}
#else
char* serialize_output(output*, unsigned int*);

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

#endif

#endif //ALGATORC_PROJECT_HPP