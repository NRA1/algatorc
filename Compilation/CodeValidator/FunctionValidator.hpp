#ifndef ALGATORC_FUNCTIONVALIDATOR_HPP
#define ALGATORC_FUNCTIONVALIDATOR_HPP
#include <string>
#include <vector>

#include "CodeValidator.hpp"


class FunctionValidator : public CodeValidator
{
public:
    virtual bool validateName(const std::string& name) const = 0;
    virtual bool validateReturnType(const std::string& type) const = 0;
    virtual bool validateArguments(const std::vector<std::string>& arguments) const = 0;

    bool validate(const CXCursor& cursor) const override;

    ~FunctionValidator() override = default;
};


#endif //ALGATORC_FUNCTIONVALIDATOR_HPP