#include "Type.hpp"
#include <sstream>

std::shared_ptr<Type> Type::getNullType() 
{
    return std::make_shared<TypeError>();
}

std::string TypeError::toString()
{
    return "Error_t";
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

std::string TypeString::toString() 
{
    return "string";
}

std::string TypeFunction::toString() 
{
    std::stringstream ss;
    ss << instrinsicType->toString() << "(";
    if(param_types.size() > 0)
    {
        ss << param_types[0]->toString();
        for(auto it = ++param_types.begin(); it != param_types.end(); ++it)
        {
            ss << (*it)->toString() << ", ";
        }
    }

    return ss.str();
}
