//
// Created by cembelentepe on 13/09/22.
//

#include <iostream>
#include "Callable.h"

FunctionUser::FunctionUser(StmtDeclFunc* func)
	: func(func)
{
}

Value FunctionUser::call(Interpreter* interpreter, std::vector<Value> args)
{
	return interpreter->runFunction(func, args);
}

FunctionNative::FunctionNative(FunctionNative::FuncPtr func)
	:func(func)
{

}

Value FunctionNative::call(Interpreter* interpreter, std::vector<Value> args)
{
	return func(interpreter, args);
}

std::unordered_map<std::string, TypePtr> FunctionNative::getNativeTypes()
{
	return nativeTypes;
}

std::unordered_map<std::string, FunctionNative::FuncPtr> FunctionNative::getNativeFuncs()
{
	return nativeFuncs;
}

#define BASIC_NATIVE_TYPE(x, y) TypeFactory::getFunction(TypeFactory::getPrimitive(PrimitiveTag::x), y)

std::unordered_map<std::string, TypePtr> FunctionNative::nativeTypes = {
	{"print", BASIC_NATIVE_TYPE(VOID, {TypeFactory::getString()})},
	{"show", BASIC_NATIVE_TYPE(VOID, {TypeFactory::getAny()})}
};

std::unordered_map<std::string, FunctionNative::FuncPtr> FunctionNative::nativeFuncs = {
	{"print", [](Interpreter* interpreter, std::vector<Value> args){
		std::cout << args[0].getDataTyped<std::string>();
	  	return Value::getVoid();
	}},
	{"show", [](Interpreter* interpreter, std::vector<Value> args){
	  std::cout << args[0].getInfo();
	  return Value::getVoid();
	}}
};