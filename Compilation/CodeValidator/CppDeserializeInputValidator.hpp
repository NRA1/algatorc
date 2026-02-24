#ifndef ALGATORC_CPPDESERIALIZEINPUTVALIDATOR_HPP
#define ALGATORC_CPPDESERIALIZEINPUTVALIDATOR_HPP
#include "FunctionValidator.hpp"


class CppDeserializeInputValidator : public FunctionValidator
{
public:
    CppDeserializeInputValidator() = default;

    [[nodiscard]] bool validateName(const std::string& name) const override;
    [[nodiscard]] bool validateReturnType(const std::string& type) const override;
    [[nodiscard]] bool validateArguments(const std::vector<std::string>& arguments) const override;
};


#endif //ALGATORC_CPPDESERIALIZEINPUTVALIDATOR_HPP