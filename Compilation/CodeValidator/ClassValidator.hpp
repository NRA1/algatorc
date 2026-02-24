#ifndef ALGATORC_CLASSVALIDATOR_HPP
#define ALGATORC_CLASSVALIDATOR_HPP
#include "CodeValidator.hpp"


class ClassValidator : public CodeValidator
{
public:
    virtual bool validateName(const std::string& name) const = 0;

    bool validate(const CXCursor& cursor) const override;
};


#endif //ALGATORC_CLASSVALIDATOR_HPP