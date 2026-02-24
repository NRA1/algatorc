#ifndef ALGATORC_OUTPUTVALIDATOR_HPP
#define ALGATORC_OUTPUTVALIDATOR_HPP
#include "ClassValidator.hpp"


class OutputValidator : public ClassValidator
{
public:
    OutputValidator() = default;

    bool validateName(const std::string& name) const override;
};


#endif //ALGATORC_OUTPUTVALIDATOR_HPP