#include "Value.h"

std::ostream& operator<<(std::ostream& os, const DataType& type)
{
	// TODO: insert return statement here
	return os;
}

std::ostream& operator<<(std::ostream& os, const Value* val)
{
	switch (val->type.type)
	{
	case ValueType::BOOL:
	case ValueType::CHAR:
		os << *(Value1b*)val;
		break;
	case ValueType::INTEGER:
	case ValueType::FLOAT:
		os << *(Value4b*)val;
		break;
	case ValueType::DOUBLE:
		os << *(Value8b*)val;
		break;
	case ValueType::STRING:
		os << *(StrValue*)val;
		break;
	case ValueType::NULL_TYPE:
		os << "NULL";
		break;
	case ValueType::VOID:
		os << "VOID";
		break;
	}
	return os;
}

bool DataType::operator==(const DataType& type)
{
	if ((this->intrinsicType == nullptr && type.intrinsicType == nullptr) || (this->intrinsicType == type.intrinsicType))
	{
		if (this->type == type.type)
			return true;
		else
			return false;
	}
	return false;
}

bool DataType::operator!=(const DataType& type)
{
	return !(*this == type);
}

void Value::setValue(const Value* value)
{
	switch (value->type.type)
	{
	case ValueType::BOOL:
		((Value1b*)this)->data.valBool = ((Value1b*)value)->data.valBool;
		break;
	case ValueType::CHAR:
		((Value1b*)this)->data.valChar = ((Value1b*)value)->data.valChar;
		break;
	case ValueType::INTEGER:
		((Value4b*)this)->data.valInt = ((Value4b*)value)->data.valInt;
		break;
	case ValueType::FLOAT:
		((Value4b*)this)->data.valFloat = ((Value4b*)value)->data.valFloat;
		break;
	case ValueType::DOUBLE:
		((Value8b*)this)->data.valDouble = ((Value8b*)value)->data.valDouble;
		break;
	case ValueType::STRING:
		((StrValue*)this)->data = ((StrValue*)value)->data;
		break;
	}

}
