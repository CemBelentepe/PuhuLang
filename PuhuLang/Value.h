#pragma once
#include <iostream>
#include <string>

class Chunk;

enum class ValueType
{
	INTEGER, FLOAT, DOUBLE, BOOL, CHAR, STRING, NULL_TYPE, VOID, FUNCTION, NATIVE, POINTER, ARRAY, ERROR
};

class DataType
{
public:
	ValueType type;
	DataType* intrinsicType;
	bool isConst;

	DataType()
		:type(ValueType::ERROR), intrinsicType(nullptr), isConst(false)
	{}

	DataType(ValueType type, bool isConst = false)
		:type(type), intrinsicType(nullptr), isConst(isConst)
	{}

	DataType(ValueType type, ValueType intrinsicType, bool isConst = false)
		:type(type), intrinsicType(new DataType(intrinsicType)), isConst(isConst)
	{}

	DataType(ValueType type, DataType* intrinsicType, bool isConst = false)
		:type(type), intrinsicType(intrinsicType), isConst(isConst)
	{}

	friend std::ostream& operator<<(std::ostream& os, const DataType& type);
	bool operator==(const DataType& type);
	bool operator!=(const DataType& type);
	inline bool isPrimitive() const { return type <= ValueType::VOID; }
};

class Value
{
public:
	DataType type;

	Value()
		:type(ValueType::ERROR)
	{}

	Value(DataType type)
		: type(type)
	{}

	virtual ~Value() {}

	friend std::ostream& operator<<(std::ostream& os, const Value* val);

	void setValue(const Value* value);
};

class Value1b : public Value
{
public:
	union u_data
	{
		bool valBool;
		char valChar;
	} data;

	Value1b(bool value)
		: Value(ValueType::BOOL)
	{
		data.valBool = value;
	}

	Value1b(char value)
		: Value(ValueType::CHAR)
	{
		data.valChar = value;
	}

	virtual ~Value1b() {}

	friend std::ostream& operator<<(std::ostream& os, const Value1b& val)
	{
		switch (val.type.type)
		{
		case ValueType::BOOL:
			if (val.data.valBool)
				os << "true";
			else
				os << "false";
			break;
		case ValueType::CHAR:
			os << val.data.valChar;
			break;
		}
		return os;
	}
};

class Value4b : public Value
{
public:
	union u_data
	{
		float valFloat;
		int32_t valInt;
	} data;

	Value4b(float value)
		: Value(ValueType::FLOAT)
	{
		data.valFloat = value;
	}

	Value4b(int32_t value)
		: Value(ValueType::INTEGER)
	{
		data.valInt = value;
	}

	virtual ~Value4b() {}

	friend std::ostream& operator<<(std::ostream& os, const Value4b& val)
	{
		switch (val.type.type)
		{
		case ValueType::FLOAT:
			os << val.data.valFloat;
			break;
		case ValueType::INTEGER:
			os << val.data.valInt;
			break;
		}
		return os;
	}
};

class Value8b : public Value
{
public:
	union u_data
	{
		double valDouble;
	} data;

	Value8b(double value)
		: Value(ValueType::DOUBLE)
	{
		data.valDouble = value;
	}

	virtual ~Value8b() {}

	friend std::ostream& operator<<(std::ostream& os, const Value8b& val)
	{
		switch (val.type.type)
		{
		case ValueType::DOUBLE:
			os << val.data.valDouble;
			break;
		}
		return os;
	}
};

class StrValue : public Value
{
public:
	std::string data;

	StrValue(const std::string& str)
		: Value(ValueType::STRING), data(str)
	{}

	StrValue(const char* str)
		: Value(ValueType::STRING), data(str)
	{}

	virtual ~StrValue() {}

	friend std::ostream& operator<<(std::ostream& os, const StrValue& val)
	{
		os << val.data;
		return os;
	}
};

class FuncValue : public Value
{
public:
	int arity;
	Chunk* chunk;

	FuncValue(Chunk* chunk, DataType returnType, int arity)
		:Value(DataType(ValueType::FUNCTION, new DataType(returnType))), arity(arity), chunk(chunk)
	{}

	FuncValue(Chunk* chunk, DataType* returnType, int arity)
		:Value(DataType(ValueType::FUNCTION, returnType)), arity(arity), chunk(chunk)
	{}

	virtual ~FuncValue()
	{
		// Delete it?
		// delete chunk;
	}
};

typedef Value* (*NativeFn)(int, Value**);
class NativeFunc : public Value
{
public:
	int arity;
	NativeFn func;

	NativeFunc(NativeFn func, DataType returnType, int arity)
		:Value(DataType(ValueType::NATIVE, new DataType(returnType))), arity(arity), func(func)
	{}

	NativeFunc(NativeFn func, DataType* returnType, int arity)
		:Value(DataType(ValueType::NATIVE, returnType)), arity(arity), func(func)
	{}

	virtual ~NativeFunc() {	}
};

class GlobalVariable : public Value
{
public:
	Value* val;
	bool initialized;

	GlobalVariable(DataType type)
		: Value(type), val(nullptr), initialized(false)
	{}

	virtual ~GlobalVariable()
	{
		// delete val;
	}
};

class LocalVariable
{
public:
	DataType type;
	std::string name;
	int depth;

	LocalVariable()
		: type(), name(), depth(0)
	{}

	LocalVariable(DataType type, std::string name, int depth)
		: type(type), name(name), depth(depth)
	{}
};