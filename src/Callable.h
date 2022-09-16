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
	explicit FunctionUser(std::unique_ptr<StmtDeclFunc> func);
	Value call(Interpreter* interpreter, std::vector<Value> args) override;

private:
	std::unique_ptr<StmtDeclFunc> func;
};

class FunctionNative: public Callable
{
public:
	using FuncPtr = auto (*)(Interpreter* interpreter, std::vector<Value> args) -> Value;

	explicit FunctionNative(FuncPtr func);
	Value call(Interpreter* interpreter, std::vector<Value> args) override;

private:
	FuncPtr func;
};