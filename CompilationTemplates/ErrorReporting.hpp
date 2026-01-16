#ifndef ALGATORC_ERRORREPORTING_HPP
#define ALGATORC_ERRORREPORTING_HPP
#include <assert.h>
#include <stdlib.h>
#include <string.h>

char* __error_var = 0;

#ifdef ALGATORCPP
extern "C"
#endif
// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
char* __error()
{
    return __error_var;
}

#ifdef ALGATORCPP
extern "C"
#endif
// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile // inline functions are not exported as symbols
void __clear_error()
{
    if (__error_var != 0)
        free(__error_var);
    __error_var = 0;
}

void __report_error(const char* error)
{
    assert(__error_var == 0);
    __error_var = strdup(error);
}


#endif //ALGATORC_ERRORREPORTING_HPP