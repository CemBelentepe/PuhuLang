#pragma once
#include "Value.h"
#include <ctime>

Value* native_print(int argc, Value** args)
{
	std::cout << (args[0]);
	return new Value(ValueType::VOID);
}

Value* native_println(int argc, Value** args)
{
	std::cout << (args[0]) << std::endl;
	return new Value(ValueType::VOID);
}

Value* native_input(int argc, Value** args)
{
	std::string* in = new std::string();
	getline(std::cin, *in);
	return new StrValue(*in);
}

Value* native_inputInt(int argc, Value** args)
{
	int32_t* i = new int32_t();
	std::cin >> *i;
	return new Value4b(*i);
}

Value* native_clock(int argc, Value** args)
{
	return new Value8b((double)clock() / CLOCKS_PER_SEC);
}