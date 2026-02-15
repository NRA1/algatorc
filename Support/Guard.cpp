#include "Guard.hpp"

#include <llvm/Support/CrashRecoveryContext.h>

void guardVoid(const std::string& source, char* (*err)(), void (*clear_err)(), const std::function<void()>& func)
{
    llvm::CrashRecoveryContext crc;
    crc.Enable();

    const bool failed = !crc.RunSafelyOnNewStack([&]()
    {
        func();
    });

    std::optional<std::string> msg;
    const char* err_msg = err();
    if (err_msg != nullptr)
    {
        msg = std::string(err_msg);
        clear_err();
    }
    else if (failed)
        msg = "Unknown failure";

    if (msg.has_value())
        error(ErrorType::User, "Failure occurred in ") << source << ": " << msg.value();

    crc.Disable();
}

template <>
void guard<void>(const std::string& source, char* (*error)(), void (*clear_error)(), const std::function<void()>& func)
{
    guardVoid(source, error, clear_error, func);
}

void guardInternal(const std::function<void()>& func)
{
    llvm::CrashRecoveryContext crc;
    crc.Enable();
    if (!crc.RunSafelyOnNewStack([&]()
    {
        func();
    }))
    {
        error(ErrorType::User, "Untrusted operation failed. This is likely due to user code changing"
                               " process-owned data in a way which is not permitted");
    }
    crc.Disable();
}
