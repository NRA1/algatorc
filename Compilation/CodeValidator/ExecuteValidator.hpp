#ifndef ALGATORC_EXECUTEVALIDATOR_HPP
#define ALGATORC_EXECUTEVALIDATOR_HPP
#include "FunctionValidator.hpp"


class ExecuteValidator : public FunctionValidator
{
public:
    ExecuteValidator() = default;

    bool validateName(const std::string& name) const override;
    bool validateReturnType(const std::string& type) const override;
    bool validateArguments(const std::vector<std::string>& arguments) const override;
};

#endif //ALGATORC_EXECUTEVALIDATOR_HPP