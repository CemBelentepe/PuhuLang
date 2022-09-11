//
// Created by cembelentepe on 08/09/22.
//

#include "Type.h"
#include <string>
#include <sstream>

std::string Type::toString()
{
	std::string res = this->toStringHook();
	if(isConst)
		res += " const";
	return res;
}

bool TypeError::isSame(const std::shared_ptr<Type>& other) const
{
	return false;
}

std::string TypeError::toStringHook()
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

std::string TypePrimitive::toStringHook()
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
		return intrinsicType && other->intrinsicType && intrinsicType->isSame(other->intrinsicType);
	}
	return false;
}

std::string TypePointer::toStringHook()
{
	if (intrinsicType)
		return intrinsicType->toString() + "*";
	else
		return "nullptr";
}

bool TypeArray::isSame(const std::shared_ptr<Type>& other) const
{
	if (other->tag == Tag::POINTER)
	{
		return intrinsicType && other->intrinsicType && intrinsicType->isSame(other->intrinsicType);
	}
	return false;
}

std::string TypeArray::toStringHook()
{
	return intrinsicType->toString() + "[]";
}

bool TypeString::isSame(const std::shared_ptr<Type>& other) const
{
	return other->tag == Type::Tag::STRING;
}

std::string TypeString::toStringHook()
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

std::string TypeFunction::toStringHook()
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

bool TypeUserDefined::isSame(const std::shared_ptr<Type>& other) const
{
	if (other->tag == Type::Tag::USER_DEF)
	{
		Token otherName = std::dynamic_pointer_cast<TypeUserDefined>(other)->name;
		return name.getLexeme() == otherName.getLexeme();
	}

	return false;
}

std::string TypeUserDefined::toStringHook()
{
	return std::string(name.lexeme);
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

TypePtr TypeFactory::getArray(const std::shared_ptr<Type>& intrinsicType)
{
	return std::make_shared<TypeArray>(intrinsicType);
}

TypePtr TypeFactory::getPointer(const std::shared_ptr<Type>& intrinsicType)
{
	return std::make_shared<TypePointer>(intrinsicType);
}

TypePtr TypeFactory::getFunction(const TypePtr& ret_type, const std::vector<TypePtr>& param_types)
{
	return std::make_shared<TypeFunction>(ret_type, param_types);
}

TypePtr TypeFactory::getUserDefined(Token name)
{
	return std::make_shared<TypeUserDefined>(name);
}

TypePtr TypeFactory::fromToken(Token token)
{
	switch (token.type)
	{
	case TokenType::VOID:
		return getPrimitive(PrimitiveTag::VOID);
	case TokenType::CHAR:
		return getPrimitive(PrimitiveTag::CHAR);
	case TokenType::INT:
		return getPrimitive(PrimitiveTag::INT);
	case TokenType::FLOAT:
		return getPrimitive(PrimitiveTag::FLOAT);
	case TokenType::DOUBLE:
		return getPrimitive(PrimitiveTag::DOUBLE);
	case TokenType::STRING:
		return getString();
	case TokenType::IDENTIFIER:
		return getUserDefined(token);
	default:
		throw std::runtime_error("Invalid token for type description.");
	}
}
