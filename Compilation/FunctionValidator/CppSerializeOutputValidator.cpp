#include "CppSerializeOutputValidator.hpp"

bool CppSerializeOutputValidator::validateName(const std::string& name) const
{
    return name == "serialize_output";
}

bool CppSerializeOutputValidator::validateReturnType(const std::string& type) const
{
    return type == "void";
}

bool CppSerializeOutputValidator::validateArguments(const std::vector<std::string>& arguments) const
{
    if (arguments.size() != 2) return false;
    if (arguments[0] != "ostream &" && arguments[0] != "std::ostream &") return false;
    return arguments[1] == "output *";
}
