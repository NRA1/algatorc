#ifndef ALGATORC_CDESERIALIZEOUTPUTVALIDATOR_HPP
#define ALGATORC_CDESERIALIZEOUTPUTVALIDATOR_HPP
#include "FunctionValidator.hpp"


class CDeserializeInputValidator : public FunctionValidator
{
public:
    CDeserializeInputValidator() = default;

    [[nodiscard]] bool validateName(const std::string& name) const override;
    [[nodiscard]] bool validateReturnType(const std::string& type) const override;
    [[nodiscard]] bool validateArguments(const std::vector<std::string>& arguments) const override;
};


#endif //ALGATORC_CDESERIALIZEOUTPUTVALIDATOR_HPP