#ifndef ALGATORC_ERROR_HANDLER_HPP
#define ALGATORC_ERROR_HANDLER_HPP
#include <optional>
#include <string>
#include <fstream>

namespace ErrorType
{
    enum ErrorType
    {
        User,
        System
    };
}

namespace ErrorPhase
{
    enum ErrorPhase
    {
        Preparation,
        Compilation,
        Setup,
        Execution,
        Teardown
    };
}

class Error
{
public:
    Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase);
    Error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase, const std::string& message);

    Error& operator<<(const std::string& message);
    Error& operator<<(const char* message);

    [[noreturn]] ~Error();

private:
    std::optional<std::ofstream> outfile_ = std::nullopt;
};

Error error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase);
Error error(ErrorType::ErrorType type, ErrorPhase::ErrorPhase phase, const std::string& message);


#endif //ALGATORC_ERROR_HANDLER_HPP