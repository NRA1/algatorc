#ifndef ALGATORC_CSERIALIZEOUTPUTVALIDATOR_HPP
#define ALGATORC_CSERIALIZEOUTPUTVALIDATOR_HPP
#include "FunctionValidator.hpp"


class CSerializeOutputValidator : public FunctionValidator
{
public:
    CSerializeOutputValidator() = default;
    [[nodiscard]] bool validateName(const std::string& name) const override;
    [[nodiscard]] bool validateReturnType(const std::string& type) const override;
    [[nodiscard]] bool validateArguments(const std::vector<std::string>& arguments) const override;
};


#endif //ALGATORC_CSERIALIZEOUTPUTVALIDATOR_HPP