#include "DummyConsumer.hpp"

void DummyConsumer::HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel, const clang::Diagnostic& Info)
{
    DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
}
