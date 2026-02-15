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
    static void setPhase(ErrorPhase::ErrorPhase phase);

    explicit Error(ErrorType::ErrorType type);
    Error(ErrorType::ErrorType type, const std::string& message);

    Error& operator<<(const std::string& message);
    Error& operator<<(const char* message);

    [[noreturn]] ~Error();

private:
    static ErrorPhase::ErrorPhase phase_;

    std::optional<std::ofstream> outfile_ = std::nullopt;
};

Error error(ErrorType::ErrorType type);
Error error(ErrorType::ErrorType type, const std::string& message);


#endif //ALGATORC_ERROR_HANDLER_HPP