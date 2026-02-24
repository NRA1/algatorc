#include "ExecuteValidator.hpp"

bool ExecuteValidator::validateName(const std::string& name) const
{
    return name == "execute";
}

bool ExecuteValidator::validateReturnType(const std::string& type) const
{
    return type == "output *" || type == "struct output *";
}

bool ExecuteValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 1) return false;
    return arguments[0] == "input *" || arguments[0] == "struct input *";
}
