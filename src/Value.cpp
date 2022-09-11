//
// Created by cembelentepe on 09/09/22.
//

#include "Value.h"
#include "Type.h"

#include <utility>
#include <stdexcept>
#include <sstream>

Value::Value()
	: data(), type(TypeFactory::getNull())
{
}

Value::Value(Value::Data data, TypePtr type)
	: data(std::move(data)), type(std::move(type))
{

}

Value::Data Value::getData() const
{
	return data;
}
TypePtr Value::getType() const
{
	return type;
}

std::string Value::getInfo() const
{
	if (type->tag == Type::Tag::STRING)
	{
		return std::get<std::string>(data);
	}
	else if(type->tag == Type::Tag::PRIMITIVE)
	{
		PrimitiveTag tag = ((TypePrimitive*)type.get())->special_tag;
		std::stringstream res;

		switch (tag)
		{
		case PrimitiveTag::VOID:
			res << "void";
			break;
		case PrimitiveTag::BOOL:
			res << (getDataTyped<bool>() ? "True" : "False");
			break;
		case PrimitiveTag::CHAR:
			res << "'" << getDataTyped<char>() << "'";
			break;
		case PrimitiveTag::INT:
			res << getDataTyped<int>();
			break;
		case PrimitiveTag::FLOAT:
			res << getDataTyped<float>();
			break;
		case PrimitiveTag::DOUBLE:
			res << getDataTyped<double>();
			break;
		}
		return res.str();
	}
	else
		throw std::runtime_error("Invalid type for a value.");
}
