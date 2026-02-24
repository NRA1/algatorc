#ifndef ALGATORC_CPPSERIALIZEOUTPUTVALIDATOR_HPP
#define ALGATORC_CPPSERIALIZEOUTPUTVALIDATOR_HPP
#include "FunctionValidator.hpp"


class CppSerializeOutputValidator : public FunctionValidator
{
public:
    CppSerializeOutputValidator() = default;

    bool validateName(const std::string& name) const override;
    bool validateReturnType(const std::string& type) const override;
    bool validateArguments(const std::vector<std::string>& arguments) const override;
};


#endif //ALGATORC_CPPSERIALIZEOUTPUTVALIDATOR_HPP