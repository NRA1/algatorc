#ifndef ALGATORC_INPUTVALIDATOR_HPP
#define ALGATORC_INPUTVALIDATOR_HPP
#include "ClassValidator.hpp"


class InputValidator : public ClassValidator
{
public:
    InputValidator() = default;

    bool validateName(const std::string& name) const override;
};


#endif //ALGATORC_INPUTVALIDATOR_HPP