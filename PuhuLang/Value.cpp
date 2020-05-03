#include "Value.h"
#include "Chunk.h"

std::ostream& operator<<(std::ostream& os, const DataType& type)
{
	// TODO: insert return statement here
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

size_t DataType::getSize()
{
	switch (type)
	{
	case ValueType::INTEGER:
		return sizeof(int32_t);
	case ValueType::FLOAT:
		return sizeof(float);
	case ValueType::DOUBLE:
		return sizeof(double);
	case ValueType::BOOL:
		return sizeof(bool);
	case ValueType::CHAR:
		return sizeof(char);
	case ValueType::STRING:
		return sizeof(const char**);
	case ValueType::NULL_TYPE:
		return 0;
	case ValueType::VOID:
		return 0;
	case ValueType::FUNCTION:
		return sizeof(Chunk*);
	case ValueType::NATIVE:
		return sizeof(NativeFunc*);
	}
	return 0;
}

void Value::setDataSized(uint8_t* data, size_t size)
{
	for (int i = 0; i < size; i++)
		((uint8_t*)(&(this->data.valDouble)))[i] = data[i];
}

uint8_t* Value::cloneData()
{
	uint8_t* clone = new uint8_t[type.getSize()];
	switch (type.type)
	{
	case ValueType::INTEGER:
		*(int32_t*)clone = data.valInt;
		break;
	case ValueType::FLOAT:
		*(float*)clone = data.valFloat;
		break;
	case ValueType::DOUBLE:
		*(double*)clone = data.valDouble;
		break;
	case ValueType::BOOL:
		*(bool*)clone = data.valBool;
		break;
	case ValueType::CHAR:
		*(char*)clone = data.valChar;
		break;
	default:
		std::cout << "Unknown type cloning";
		break;
	}
		return clone;
}

uint8_t* StrValue::cloneData()
{
	char** clone = new char* ();
	*clone = new char[data.size() + 1];
	strcpy_s(*clone, data.size() + 1, data.c_str());
	return (uint8_t*)(clone);
}

uint8_t* FuncValue::cloneData()
{
#ifdef _DEBUG
	std::cout << "Chunk: " << chunk << std::endl;
#endif // _DEBUG

	Chunk*** clone = new Chunk**();
	*clone = new Chunk*();
	**clone = chunk;
	return (uint8_t*)(clone);
}

uint8_t* NativeFunc::cloneData()
{
	NativeFunc*** clone = new NativeFunc**();
	*clone = new NativeFunc *();
	**clone = new NativeFunc(this->func, this->type, this->arity);
	(**clone)->type = this->type;
#ifdef _DEBUG
	std::cout << "Native: " << **clone << "\n";
#endif // _DEBUG

	return (uint8_t*)(clone);
}
