#include "Type.hpp"
#include <sstream>

std::shared_ptr<Type> Type::getNullType()
{
    return std::make_shared<TypeError>();
}

bool TypeError::isSame(const std::shared_ptr<Type>& other) const
{
    return false;
}

std::string TypeError::toString()
{
    return "Error_t";
}

bool TypePrimitive::isSame(const std::shared_ptr<Type>& other) const
{
    if (other->tag == Type::Tag::PRIMITIVE)
    {
        return std::dynamic_pointer_cast<TypePrimitive>(other)->special_tag == special_tag;
    }
    return false;
}

std::string TypePrimitive::toString()
{
    switch (special_tag)
    {
    case PrimitiveTag::VOID:
        return "void";
    case PrimitiveTag::BOOL:
        return "bool";
    case PrimitiveTag::CHAR:
        return "char";
    case PrimitiveTag::INT:
        return "int";
    case PrimitiveTag::FLOAT:
        return "float";
    case PrimitiveTag::DOUBLE:
        return "double";
    }
}

bool TypeString::isSame(const std::shared_ptr<Type>& other) const
{
    return other->tag == Type::Tag::STRING;
}

std::string TypeString::toString()
{
    return "string";
}

bool TypeFunction::isSame(const std::shared_ptr<Type>& other) const
{
    if (other->tag == Type::Tag::FUNCTION)
    {
        auto other_param_types = std::dynamic_pointer_cast<TypeFunction>(other)->param_types;
        if (other_param_types.size() != param_types.size())
            return false;
        for (size_t i = 0; i < param_types.size(); i++)
        {
            if (!param_types[i]->isSame(other_param_types[i]))
                return false;
        }
        return true;
    }
    return false;
}

std::string TypeFunction::toString()
{
    std::stringstream ss;
    ss << instrinsicType->toString() << "(";
    if (param_types.size() > 0)
    {
        ss << param_types[0]->toString();
        for (auto it = ++param_types.begin(); it != param_types.end(); ++it)
        {
            ss << ", " << (*it)->toString();
        }
    }
    ss << ")";
    return ss.str();
}