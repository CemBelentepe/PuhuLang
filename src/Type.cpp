//
// Created by cembelentepe on 08/09/22.
//

#include "Type.h"
#include <string>
#include <sstream>

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
	return "[invalid primitive type]";
}

bool TypePointer::isSame(const std::shared_ptr<Type>& other) const
{
	if (other->tag == Tag::POINTER)
	{
		return !intrinsicType || !other->intrinsicType || intrinsicType->isSame(other->intrinsicType);
	}
	return false;
}

std::string TypePointer::toString()
{
	if (intrinsicType)
	{
		if(isOwner)
			return intrinsicType->toString() + "&";
		else
			return intrinsicType->toString() + "*";
	}
	else
	{
		return "nullptr";
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
	ss << intrinsicType->toString() << "(";
	if (!param_types.empty())
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

TypePtr TypeFactory::getNull()
{
	return std::make_shared<TypeError>();
}

TypePtr TypeFactory::getPrimitive(PrimitiveTag tag)
{
	return std::make_shared<TypePrimitive>(tag);
}
TypePtr TypeFactory::getString()
{
	return std::make_shared<TypeString>();
}

