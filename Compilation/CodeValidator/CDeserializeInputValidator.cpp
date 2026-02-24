#include "CDeserializeInputValidator.hpp"

bool CDeserializeInputValidator::validateName(const std::string& name) const
{
    return name == "deserialize_input";
}

bool CDeserializeInputValidator::validateReturnType(const std::string& type) const
{
    return type == "input *" || type == "struct input *";
}

bool CDeserializeInputValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 2) return false;
    if (arguments[0] != "char *") return false;
    return arguments[1] == "unsigned int";
}
