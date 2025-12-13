#include "Error.hpp"

#include <iostream>

Error::Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase)
{
    // TODO: write to file
}

Error::Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase, const std::string& message) : Error(type, phase)
{
    std::cout << message;
    // TODO: write to file
}

Error& Error::operator<<(const std::string& message)
{
    std::cout << message;
    return *this;
}

Error& Error::operator<<(const char* message)
{
    std::cout << message;
    return *this;
}

Error::~Error()
{
    std::cout << std::endl;
    exit(1);
}

Error error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase)
{
    return {type, phase};
}

Error error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase, const std::string& message)
{
    return {type, phase, message};
}
