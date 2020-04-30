#include "Value.h"

std::ostream& operator<<(std::ostream& os, const DataType& type)
{
	// TODO: insert return statement here
	return os;
}

std::ostream& operator<<(std::ostream& os, const Value& val)
{
	switch (val.type.type)
	{
	case ValueType::INTEGER:
		os << *((int*)val.data);
		break;
	case ValueType::FLOAT:
		os <<*((float*)val.data) << "f";
		break;
	case ValueType::DOUBLE:
		os << *((double*)val.data) << "d";
		break;
	case ValueType::BOOL:
		if (*((bool*)val.data))
			os << "true";
		else
			os << "false";
		break;
	case ValueType::CHAR:
		os << *((char*)val.data);
		break;
	case ValueType::STRING:
		os << *(std::string*)val.data;
		break;
	case ValueType::VOID:
		os << "void";
		break;
	default:
		os << val.data;
		break;
	}
	return os;
}

void Value::DeleteValue()
{
	switch (type.type)
	{
	case ValueType::INTEGER:
		delete (int*)data;
		break;
	case ValueType::FLOAT:
		delete (float*)data;
		break;
	case ValueType::DOUBLE:
		delete (double*)data;
		break;
	case ValueType::BOOL:
		delete (bool*)data;
		break;
	case ValueType::STRING:
		delete (std::string*)data;
		break;
	case ValueType::CHAR:
		delete (char*)data;
		break;
	}
}

void Value::setData(void* newData)
{
	switch (type.type)
	{
	case ValueType::INTEGER:
		*(int*)this->data = *(int*)newData;
		break;
	case ValueType::FLOAT:
		*(float*)this->data = *(float*)newData;
		break;
	case ValueType::DOUBLE:
		*(double*)this->data = *(double*)newData;
		break;
	case ValueType::BOOL:
		*(bool*)this->data = *(bool*)newData;
		break;
	case ValueType::STRING:
		*(std::string*)this->data = *(std::string*)newData;
		break;
	case ValueType::CHAR:
		*(char*)this->data = *(char*)newData;
		break;
	default:
		this->data = newData;
	}
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