#ifndef ALGATORC_DUMMY_CONSUMER_HPP
#define ALGATORC_DUMMY_CONSUMER_HPP
#include <clang/Basic/Diagnostic.h>


class DummyConsumer final : public clang::DiagnosticConsumer
{
public:
    virtual void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel, const clang::Diagnostic& Info) override;
};


#endif //ALGATORC_DUMMY_CONSUMER_HPP