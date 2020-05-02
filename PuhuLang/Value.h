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

	size_t getSize();
};

class Value
{
public:
	DataType type;
	union Data
	{
		bool valBool;
		char valChar;
		int32_t valInt;
		float valFloat;
		double valDouble;
	}data;

	Value()
		:type(ValueType::ERROR), data({ 0 })
	{}

	Value(DataType type)
		: type(type), data({ 0 })
	{}

	Value(bool value)
		: type(ValueType::BOOL)
	{
		data.valBool = value;
	}

	Value(char value)
		: type(ValueType::CHAR)
	{
		data.valChar = value;
	}

	Value(int32_t value)
		: type(ValueType::INTEGER)
	{
		data.valInt = value;
	}

	Value(float value)
		: type(ValueType::FLOAT)
	{
		data.valFloat = value;
	}

	Value(double value)
		: type(ValueType::DOUBLE)
	{
		data.valDouble = value;
	}

	virtual ~Value() {}

	friend std::ostream& operator<<(std::ostream& os, const Value& val);

	virtual uint8_t* cloneData(); // Turns data into an array
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

	virtual uint8_t* cloneData();
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

	virtual uint8_t* cloneData();
};

typedef uint8_t* (*NativeFn)(int, uint8_t*);
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

	virtual ~NativeFunc() {}

	virtual uint8_t* cloneData();
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
	size_t startPos;

	LocalVariable(){}

	LocalVariable(DataType type, std::string name, int depth, size_t startPos)
		: type(type), name(name), depth(depth), startPos(startPos)
	{}
};