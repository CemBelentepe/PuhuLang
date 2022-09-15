//
// Created by cembelentepe on 13/09/22.
//

#pragma once

#include "Interpreter.h"

class Callable
{
public:
	virtual void call(Interpreter* interpreter, std::vector<Value> args) = 0;
};

class FunctionUser: public Callable
{
public:
	FunctionUser(std::unique_ptr<Stmt> body);
	void call(Interpreter* interpreter, std::vector<Value> args) override;

private:
	std::unique_ptr<Stmt> body;
};

class FunctionNative: public Callable
{
public:
	using FuncPtr = auto (*)(Interpreter* interpreter, std::vector<Value> args) -> Value;

	FunctionNative(FuncPtr func);
	void call(Interpreter* interpreter, std::vector<Value> args) override;
private:
	FuncPtr func;
};