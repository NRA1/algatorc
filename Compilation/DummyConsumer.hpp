#ifndef ALGATORC_DUMMY_CONSUMER_HPP
#define ALGATORC_DUMMY_CONSUMER_HPP
#include <clang/Basic/Diagnostic.h>


class DummyConsumer final : public clang::DiagnosticConsumer
{
};


#endif //ALGATORC_DUMMY_CONSUMER_HPP