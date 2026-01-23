#include "Error.hpp"

#include <iostream>

#include "Configuration.hpp"

Error::Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase) : outfile_(std::ofstream{})
{
    if (!Configuration::initialized())
    {
        outfile_ = std::nullopt;
        return;
    }

    outfile_->open(Configuration::statusFilePath(), std::ios::out | std::ios::trunc);
    if (!outfile_->is_open())
    {
        std::cerr << "Failed to open status file " << Configuration::statusFilePath() << " to report error:" << std::endl;
        outfile_ = std::nullopt;
        return;
    }

    outfile_.value() << "ERROR" << "\n";
    outfile_.value() << "type: ";
    switch (type)
    {
    case ErrorType::User:
        outfile_.value() << "USER";
        break;
    case ErrorType::System:
        outfile_.value() << "SYSTEM";
        break;
    }
    outfile_.value() << "\n" << "phase: ";
    switch (phase)
    {
    case ErrorPhase::Preparation:
        outfile_.value() << "PREPARATION";
        break;
    case ErrorPhase::Compilation:
        outfile_.value() << "COMPILATION";
        break;
    case ErrorPhase::Setup:
        outfile_.value() << "SETUP";
        break;
    case ErrorPhase::Execution:
        outfile_.value() << "EXECUTION";
        break;
    case ErrorPhase::Teardown:
        outfile_.value() << "TEARDOWN";
        break;
    }
    outfile_.value() << "\n";
}

Error::Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase, const std::string& message) : Error(type, phase)
{
    std::cerr << message;
    if (outfile_.has_value())
        outfile_.value() << message;
}

Error& Error::operator<<(const std::string& message)
{
    std::cerr << message;
    if (outfile_.has_value())
        outfile_.value() << message;
    return *this;
}

Error& Error::operator<<(const char* message)
{
    std::cerr << message;
    if (outfile_.has_value())
        outfile_.value() << message;
    return *this;
}

Error::~Error()
{
    std::cerr << std::endl;
    if (outfile_.has_value())
    {
        outfile_->close();
        if (outfile_->fail())
            std::cerr << "Failed to write status file: " << Configuration::statusFilePath();
    }
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
