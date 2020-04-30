#pragma once
#include <iostream>
#include <string>

class Chunk;

enum class ValueType
{
	INTEGER, FLOAT, DOUBLE, BOOL, STRING, CHAR, NULL_TYPE, VOID, FUNCTION, NATIVE, POINTER, ARRAY, ERROR
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
	void* data;
	DataType type;

	Value()
		:data(nullptr), type(ValueType::ERROR)
	{}

	Value(void* data, DataType type)
		: data(data), type(type)
	{}

	void DeleteValue();

	void setData(void* newData);

	friend std::ostream& operator<<(std::ostream& os, const Value& val);
};

class FuncValue : public Value
{
public:
	int arity;

	FuncValue(Chunk* chunk, DataType returnType, int arity)
		:Value(chunk, DataType(ValueType::FUNCTION, new DataType(returnType))), arity(arity)
	{}
};

typedef Value*(*NativeFn)(int, Value**);
class NativeFunc : public Value
{
public:
	int arity;

	NativeFunc(NativeFn func, DataType returnType, int arity)
		:Value(func, DataType(ValueType::NATIVE, new DataType(returnType))), arity(arity)
	{}
};

class GlobalVariable : public Value
{
public:
	bool initialized;

	GlobalVariable(void* data, DataType type)
		: Value(data, type), initialized(false)
	{}
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