#include "Guard.hpp"

#include <assert.h>
#include <csetjmp>
#include <csignal>
#include <mutex>
#include <thread>

#include "../ALGExit/Algexit.hpp"

#ifdef WIN32
#include <windows.h>
#endif

struct GuardContextImpl;

/// Crash recovery helper object.
///
/// This class implements support for running operations in a safe context so
/// that crashes (memory errors, stack overflow, assertion violations) can be
/// detected and control restored to the crashing thread. Crash detection is
/// purely "best effort", the exact set of failures which can be recovered from
/// is platform dependent.
class GuardContext
{
public:
    GuardContext();
    ~GuardContext();

    /// Enable crash recovery.
    static void enable();
    /// Disable crash recovery.
    static void disable();

    /// Return the active context, if the code is currently executing in a
    /// thread which is in a protected context.
    static GuardContext* getCurrent();

    /// Execute the provided callback function (with the given arguments) in
    /// a protected context.
    ///
    /// \return True if the function completed successfully, and false if the
    /// function crashed (or HandleCrash was called explicitly). Clients should
    /// make as little assumptions as possible about the program state when
    /// RunSafely has returned false.
    bool runSafely(const std::function<void()>& func, const std::function<void()>& on_failure);

    /// Execute the provide callback function (with the given arguments) in
    /// a protected context which is run in another thread.
    bool runSafelyOnThread(const std::function<void()>& func, const std::function<void()>& on_failure);

    /// In case of a crash, this is the crash identifier.
    int ret_code = 0;

    const char* failure_name = nullptr;

private:
    GuardContextImpl* impl_ = nullptr;
};

static thread_local const GuardContextImpl* g_current_context;

struct GuardContextImpl
{
    GuardContext* ctx;
    ::jmp_buf jump_buffer;
    std::function<void()> on_failure;
    volatile unsigned failed : 1;
    unsigned switched_thread : 1;
    unsigned valid_jump_buffer : 1;

    GuardContextImpl(GuardContext* ctx, const std::function<void()>& on_fail) noexcept : ctx(ctx), on_failure(on_fail),
        failed(false), switched_thread(false), valid_jump_buffer(false)
    {
        g_current_context = this;
    }

    ~GuardContextImpl()
    {
        if (!switched_thread)
            g_current_context = nullptr;
    }

    void setSwitchedThread()
    {
        switched_thread = true;
    }

    // If the function ran by the GuardContext crashes or fails, then
    // 'ret_code' represents the returned error code, as if it was returned by a
    // process.
    void handleCrash(const int ret_code, const char* failure_name)
    {
        g_current_context = nullptr;

        assert(!failed && "Guard context already failed!");
        failed = true;

        ctx->ret_code = ret_code;
        ctx->failure_name = failure_name;

        on_failure();

        // Jump back to the RunSafely we were called under.
        if (valid_jump_buffer)
            longjmp(jump_buffer, 1);
    }
};

std::mutex &getCrashRecoveryContextMutex()
{
    static std::mutex mutex;
    return mutex;
}

static bool g_crash_recovery_enabled = false;

#ifdef __linux__
// Generic POSIX implementation.
//
// This implementation relies on synchronous signals being delivered to the
// current thread. We use a thread local object to keep track of the active
// crash recovery context, and install signal handlers to invoke HandleCrash on
// the active object.
//
// This implementation does not attempt to chain signal handlers in any
// reliable fashion -- if we get a signal outside of a crash recovery context we
// simply disable crash recovery and raise the signal again.

static const int g_signals[] = {SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGSEGV, SIGTRAP, SIGSYS};
static const char* g_signal_names[] = {"SIGABRT", "SIGBUS", "SIGFPE", "SIGILL", "SIGSEGV", "SIGTRAP", "SIGSYS"};
static constexpr unsigned g_num_signals = std::size(g_signals);
static struct sigaction g_prev_actions[g_num_signals];

void disableSystemDialogsOnCrash() {}

static void crashRecoverySignalHandler(const int signal)
{
  // Lookup the current thread local recovery object.
    const GuardContextImpl* impl = g_current_context;

    if (!impl)
    {
        // We didn't find a crash recovery context -- this means either we got a
        // signal on a thread we didn't expect it on, the application got a signal
        // outside of a crash recovery context, or something else went horribly
        // wrong.
        //
        // Disable crash recovery and raise the signal again. The assumption here is
        // that the enclosing application will terminate soon, and we won't want to
        // attempt crash recovery again.
        //
        // This call of Disable isn't thread safe, but it doesn't actually matter.
        GuardContext::disable();
        raise(signal);

        // The signal will be thrown once the signal mask is restored.
        return;
    }

    // Unblock the signal we received.
    sigset_t sig_mask;
    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, signal);
    sigprocmask(SIG_UNBLOCK, &sig_mask, nullptr);

    // Return the same error code as if the program crashed, as mentioned in the
    // section "Exit Status for Commands":
    // https://pubs.opengroup.org/onlinepubs/9699919799/xrat/V4_xcu_chap02.html
    const int ret_code = 128 + signal;

    const char* signal_name = nullptr;
    for (int i = 0; i != g_num_signals; ++i)
    {
        if (g_signals[i] == signal)
        {
            signal_name = g_signal_names[i];
            break;
        }
    }

    const_cast<GuardContextImpl*>(impl)->handleCrash(ret_code, signal_name);
}

static void installExceptionOrSignalHandlers()
{
    // Setup the signal handler.
    struct sigaction handler{};
    handler.sa_handler = crashRecoverySignalHandler;
    handler.sa_flags = 0;
    sigemptyset(&handler.sa_mask);

    for (unsigned i = 0; i != g_num_signals; ++i)
        sigaction(g_signals[i], &handler, &g_prev_actions[i]);
}

static void uninstallExceptionOrSignalHandlers()
{
    // Restore the previous signal handlers.
    for (unsigned i = 0; i != g_num_signals; ++i)
        sigaction(g_signals[i], &g_prev_actions[i], nullptr);
}
#elifdef WIN32

void disableSystemDialogsOnCrash()
{
    // Disable standard error dialog box.
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
                 SEM_NOOPENFILEERRORBOX);
    _set_error_mode(_OUT_TO_STDERR);
}

static LONG CALLBACK exceptionHandler(LPEXCEPTION_POINTERS ExceptionInfo)
{
    // DBG_PRINTEXCEPTION_WIDE_C is not properly defined on all supported
    // compilers and platforms, so we define it manually.
    constexpr ULONG DbgPrintExceptionWideC = 0x4001000AL;
    switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case DBG_PRINTEXCEPTION_C:
    case DbgPrintExceptionWideC:
    case 0x406D1388:  // set debugger thread name
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // Lookup the current thread local recovery object.
    const GuardContextImpl *impl = g_current_context;

    if (!impl) {
        // Something has gone horribly wrong, so let's just tell everyone
        // to keep searching
        GuardContext::disable();
        return EXCEPTION_CONTINUE_SEARCH;
    }

    int ret_code = (int)ExceptionInfo->ExceptionRecord->ExceptionCode;
    if ((ret_code & 0xF0000000) == 0xE0000000)
        ret_code &= ~0xF0000000; // this crash was generated by sys::Process::Exit

    const char* failure_name = nullptr;
    if (ret_code == EXCEPTION_ACCESS_VIOLATION)
        failure_name = "ACCESS_VIOLATION";
    else if (ret_code == EXCEPTION_ARRAY_BOUNDS_EXCEEDED)
        failure_name = "ARRAY_BOUNDS_EXCEEDED";
    else if (ret_code == EXCEPTION_DATATYPE_MISALIGNMENT)
        failure_name = "DATATYPE_MISALIGNMENT";
    else if (ret_code == EXCEPTION_FLT_DENORMAL_OPERAND)
        failure_name = "FLT_DENORMAL_OPERAND";
    else if (ret_code == EXCEPTION_FLT_DIVIDE_BY_ZERO)
        failure_name = "FLT_DIVIDE_BY_ZERO";
    else if (ret_code == EXCEPTION_FLT_INEXACT_RESULT)
        failure_name = "FLT_INEXACT_RESULT";
    else if (ret_code == EXCEPTION_FLT_INVALID_OPERATION)
        failure_name = "FLT_INVALID_OPERATION";
    else if (ret_code == EXCEPTION_FLT_OVERFLOW)
        failure_name = "FLT_OVERFLOW";
    else if (ret_code == EXCEPTION_FLT_STACK_CHECK)
        failure_name = "FLT_STACK_CHECK";
    else if (ret_code == EXCEPTION_FLT_UNDERFLOW)
        failure_name = "FLT_UNDERFLOW";
    else if (ret_code == EXCEPTION_ILLEGAL_INSTRUCTION)
        failure_name = "ILLEGAL_INSTRUCTION";
    else if (ret_code == EXCEPTION_IN_PAGE_ERROR)
        failure_name = "IN_PAGE_ERROR";
    else if (ret_code == EXCEPTION_INT_DIVIDE_BY_ZERO)
        failure_name = "INT_DIVIDE_BY_ZERO";
    else if (ret_code == EXCEPTION_INT_OVERFLOW)
        failure_name = "INT_OVERFLOW";
    else if (ret_code == EXCEPTION_INVALID_DISPOSITION)
        failure_name = "INVALID_DISPOSITION";
    else if (ret_code == EXCEPTION_NONCONTINUABLE_EXCEPTION)
        failure_name = "NONCONTINUABLE_EXCEPTION";
    else if (ret_code == EXCEPTION_PRIV_INSTRUCTION)
        failure_name = "PRIV_INSTRUCTION";
    else if (ret_code == EXCEPTION_STACK_OVERFLOW)
        failure_name = "STACK_OVERFLOW";

    // Handle the crash
    const_cast<GuardContextImpl*>(impl)->handleCrash(
        ret_code, failure_name);

    // Note that we don't actually get here because HandleCrash calls
    // longjmp, which means the HandleCrash function never returns.
    abort(); //Unreachable
}

static thread_local LPTOP_LEVEL_EXCEPTION_FILTER g_prev_exception_filter = nullptr;

static void installExceptionOrSignalHandlers()
{
    const LPTOP_LEVEL_EXCEPTION_FILTER filter = ::SetUnhandledExceptionFilter(exceptionHandler);
    g_prev_exception_filter = filter;
}

static void uninstallExceptionOrSignalHandlers()
{
    LPTOP_LEVEL_EXCEPTION_FILTER prev_filter = g_prev_exception_filter;
    if (prev_filter) {
        // Now we can remove the vectored exception handler from the chain
        ::SetUnhandledExceptionFilter(prev_filter);

        // Reset the handle in our thread-local set.
        g_prev_exception_filter = nullptr;
    }
}
#endif

GuardContext::GuardContext()
{
    // On Windows, if abort() was previously triggered (and caught by a previous
    // GuardContext) the Windows CRT removes our installed signal handler,
    // so we need to install it again.
    disableSystemDialogsOnCrash();
}

GuardContext::~GuardContext()
{
    //
    delete impl_;
    impl_ = nullptr;
}

GuardContext* GuardContext::getCurrent()
{
    if (!g_crash_recovery_enabled) return nullptr;

    const GuardContextImpl* impl = g_current_context;
    if (!impl) return nullptr;

    return impl->ctx;
}

void GuardContext::enable()
{
    std::lock_guard<std::mutex> lock(getCrashRecoveryContextMutex());
    if (g_crash_recovery_enabled) return;
    g_crash_recovery_enabled = true;
    installExceptionOrSignalHandlers();
}

void GuardContext::disable()
{
    std::lock_guard<std::mutex> lock(getCrashRecoveryContextMutex());
    if (!g_crash_recovery_enabled) return;
    g_crash_recovery_enabled = false;
    uninstallExceptionOrSignalHandlers();
}

bool GuardContext::runSafely(const std::function<void()>& func, const std::function<void()>& on_failure)
{
    // If crash recovery is disabled, do nothing.
    if (g_crash_recovery_enabled)
    {
        assert(!impl_ && "Crash recovery context already initialized!");
        GuardContextImpl* impl = new GuardContextImpl(this, on_failure);
        impl_ = impl;

        impl->valid_jump_buffer = true;
        if (setjmp(impl->jump_buffer) != 0) return false;
    }

    func();
    return true;
}

struct RunSafelyOnThreadInfo
{
    std::function<void()> func;
    std::function<void()> on_failure;
    GuardContext* ctx;
    bool result;
};

static void runSafelyOnThreadDispatch(RunSafelyOnThreadInfo* info)
{
    info->result = info->ctx->runSafely(info->func, info->on_failure);
}

bool GuardContext::runSafelyOnThread(const std::function<void()>& func, const std::function<void()>& on_failure)
{
    RunSafelyOnThreadInfo info = {func, on_failure, this, false};
    std::thread thread(runSafelyOnThreadDispatch, &info);
    thread.join();

    impl_->setSwitchedThread();
    return info.result;
}


void guardVoid(const std::string& source, const std::function<char*()>& err, const std::function<void()>& clear_err,
    const std::function<void()>& func, const std::function<void()>& on_failure)
{
    GuardContext ctx;
    GuardContext::enable();

    bool termination_attempted = false;
    int termination_exit_code = 0;
    const bool failed = !terminationInterceptorApply<bool>([&]
    {
        return ctx.runSafelyOnThread([&]()
       {
           func();
       }, on_failure);
    }, termination_attempted, termination_exit_code);

    std::optional<std::string> msg;
    const char* err_msg = err();
    if (err_msg != nullptr)
    {
        msg = std::string(err_msg);
        clear_err();
    }
    else if (termination_attempted)
    {
        msg = std::string("Termination attempted with exit code ") + std::to_string(termination_exit_code);
    }
    else if (failed)
    {
        if (ctx.failure_name != nullptr)
        {
            msg = std::string(ctx.failure_name);
        }
        else msg = "Unknown failure";
    }

    if (msg.has_value())
        error(ErrorType::User, "Failure occurred in \"") << source << "\": " << msg.value();

    GuardContext::disable();
}

template <>
void guard<void>(const std::string& source, const std::function<char*()>& err, const std::function<void()>& clear_err
    , const std::function<void()>& func, const std::function<void()>& on_failure)
{
    guardVoid(source, err, clear_err, func, on_failure);
}

void guardInternal(const std::function<void()>& func)
{
    GuardContext ctx;
    GuardContext::enable();
    if (!ctx.runSafely([&]()
    {
        func();
    }, []{}))
    {
        error(ErrorType::User, "Untrusted operation failed. This is likely due to user code changing"
                               " process-owned data in a way which is not permitted");
    }
    GuardContext::disable();
}