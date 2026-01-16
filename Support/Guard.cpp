#include "Guard.hpp"

#include <llvm/Support/CrashRecoveryContext.h>

void guardVoid(const std::string& source, char* (*err)(), void (*clear_err)(), const std::function<void()>& func)
{
    // llvm::CrashRecoveryContext crc;
    // crc.Enable();

    // if (!crc.RunSafelyOnNewStack([&]()
    // {
        func();
    // }))
    // {
        // std::string msg;
        // const char* err_msg = err();
        // if (err_msg != nullptr)
        // {
            // msg = std::string(err_msg);
            // clear_err();
        // }
        // else msg = "Unknown failure";

        // error(ErrorType::User, ErrorPhase::Execution, "Failure occurred in ") << source << ": " << msg;
    // }
    // crc.Disable();
}

template <>
void guard<void>(const std::string& source, char* (*error)(), void (*clear_error)(), const std::function<void()>& func)
{
    guardVoid(source, error, clear_error, func);
}
