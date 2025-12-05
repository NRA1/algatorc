#ifndef ALGATORC_ERROR_HANDLER_HPP
#define ALGATORC_ERROR_HANDLER_HPP
#include <optional>
#include <string>
#include <fstream>

class error
{
public:
    enum error_type
    {
        User,
        System
    };

    enum error_phase
    {
        Preparation,
        Compilation,
        Setup,
        Execution,
        Teardown
    };

    error(error_type type, error_phase phase);
    error(error_type type, error_phase phase, std::string message);

    error& operator<<(const std::string& message);
    error& operator<<(const char* message);

    [[noreturn]] ~error();

private:

    std::optional<std::ofstream> m_outfile = std::nullopt;
};


#endif //ALGATORC_ERROR_HANDLER_HPP