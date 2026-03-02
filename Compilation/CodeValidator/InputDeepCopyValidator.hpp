#ifndef ALGATORC_INPUTDEEPCOPYVALIDATOR_HPP
#define ALGATORC_INPUTDEEPCOPYVALIDATOR_HPP
#include "FunctionValidator.hpp"


class InputDeepCopyValidator : public FunctionValidator
{
public:
    bool validateName(const std::string& name) const override;
    bool validateReturnType(const std::string& type) const override;
    bool validateArguments(const std::vector<std::string>& arguments) const override;
};


#endif //ALGATORC_INPUTDEEPCOPYVALIDATOR_HPP