//
// Created by cembelentepe on 13/09/22.
//

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
