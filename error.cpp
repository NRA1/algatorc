#include "error.hpp"

#include <iostream>

error::error(error_type type, error_phase phase)
{
    // TODO: write to file
}

error::error(error_type type, error_phase phase, std::string message) : error(type, phase)
{
    std::cout << message;
    // TODO: write to file
}

error& error::operator<<(const std::string& message)
{
    std::cout << message;
    return *this;
}

error& error::operator<<(const char* message)
{
    std::cout << message;
    return *this;
}

error::~error()
{
    std::cout << std::endl;
    exit(1);
}
