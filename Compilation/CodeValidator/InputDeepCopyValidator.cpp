#include "InputDeepCopyValidator.hpp"

bool InputDeepCopyValidator::validateName(const std::string& name) const
{
    return name == "input_deep_copy";
}

bool InputDeepCopyValidator::validateReturnType(const std::string& type) const
{
    return type == "struct input *" || type == "input *";
}

bool InputDeepCopyValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 1) return false;
    return arguments[0] == "const struct input *" || arguments[0] == "const input *";
}
