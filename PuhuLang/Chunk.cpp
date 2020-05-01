#include "Chunk.h"

Chunk::Chunk()
{}

Chunk::~Chunk()
{
	for (auto& val : constants)
	{
		delete val;
	}
}

size_t Chunk::addConstant(Value* value)
{
	// TODO: check before add
	bool contains = false;
	int pos = -1;
	for (int i = 0; i < this->constants.size() && !contains; i++)
	{
		auto& c = this->constants[i];
		if (this->constants[i]->type == value->type)
		{
			switch (value->type.type)
			{
			case ValueType::INTEGER:
				contains = ((Value4b*)value)->data.valInt == ((Value4b*)c)->data.valInt;
				break;
			case ValueType::FLOAT:
				contains = ((Value4b*)value)->data.valFloat == ((Value4b*)c)->data.valFloat;
				break;
			case ValueType::DOUBLE:
				contains = ((Value8b*)value)->data.valDouble == ((Value8b*)c)->data.valDouble;
				break;
			case ValueType::BOOL:
				contains = ((Value1b*)value)->data.valBool == ((Value1b*)c)->data.valBool;
				break;
			case ValueType::CHAR:
				contains = ((Value1b*)value)->data.valChar == ((Value1b*)c)->data.valChar;
				break;
			case ValueType::STRING:
				contains = ((StrValue*)value)->data == ((StrValue*)c)->data;
				break;
			case ValueType::NULL_TYPE:
				contains = true;
				break;
			case ValueType::VOID:
				contains = true;
				break;
			case ValueType::FUNCTION:
				contains = ((FuncValue*)value)->chunk == ((FuncValue*)c)->chunk;
				break;
			case ValueType::NATIVE:
				contains = ((NativeFunc*)value)->func == ((NativeFunc*)c)->func;
				break;
			default:
				contains = false;
				break;
			}
			if (contains)
				pos = i;
		}
	}

	if (contains)
	{
		delete value;
		return pos;
	}
	else
	{
		this->constants.push_back(value);
		return constants.size() - 1;
	}

}

Value* Chunk::getConstant(size_t addr)
{
	return this->constants[addr];
}
