#include "CppDeserializeInputValidator.hpp"

bool CppDeserializeInputValidator::validateName(const std::string& name) const
{
    return name == "deserialize_input";
}

bool CppDeserializeInputValidator::validateReturnType(const std::string& type) const
{
    return type == "input *";
}

bool CppDeserializeInputValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 1) return false;
    return arguments[0] == "istream &" || arguments[0] == "std::istream &";
}
