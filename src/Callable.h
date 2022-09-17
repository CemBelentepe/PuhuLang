//
// Created by cembelentepe on 13/09/22.
//

#pragma once

#include "Interpreter.h"

class Callable
{
public:
	virtual Value call(Interpreter* interpreter, std::vector<Value> args) = 0;
};

class FunctionUser: public Callable
{
public:
	explicit FunctionUser(StmtDeclFunc* func);
	Value call(Interpreter* interpreter, std::vector<Value> args) override;

private:
	StmtDeclFunc* func;
};

class FunctionNative: public Callable
{
public:
	using FuncPtr = auto (*)(Interpreter* interpreter, std::vector<Value> args) -> Value;

	explicit FunctionNative(FuncPtr func);
	Value call(Interpreter* interpreter, std::vector<Value> args) override;

	static std::unordered_map<std::string, TypePtr> getNativeTypes();
	static std::unordered_map<std::string, FuncPtr> getNativeFuncs();

private:
	FuncPtr func;
	static std::unordered_map<std::string, TypePtr> nativeTypes;
	static std::unordered_map<std::string, FuncPtr> nativeFuncs;
};