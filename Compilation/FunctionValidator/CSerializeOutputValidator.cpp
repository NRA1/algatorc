#include "CSerializeOutputValidator.hpp"

bool CSerializeOutputValidator::validateName(const std::string& name) const
{
    return name == "serialize_output";
}

bool CSerializeOutputValidator::validateReturnType(const std::string& type) const
{
    return type == "char *";
}

bool CSerializeOutputValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 2) return false;
    if (arguments[0] != "output *") return false;
    return arguments[1] == "unsigned int *";
}

