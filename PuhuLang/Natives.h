#pragma once
#include "Value.h"
#include <ctime>

uint8_t* native_print(int argc, uint8_t* args)
{
	std::cout << *(char**)(args);
	return nullptr;
}

uint8_t* native_println(int argc, uint8_t* args)
{
	puts(*(char**)(args));
	return nullptr;
}

uint8_t* native_printlnDouble(int argc, uint8_t* args)
{
	std::cout << *(double*)args << "\n";
	return nullptr;
}

uint8_t* native_input(int argc, uint8_t* args)
{
	std::string in;
	getline(std::cin, in);
	return StrValue(in).cloneData();
}

uint8_t* native_inputInt(int argc, uint8_t* args)
{
	int32_t i;
	std::cin >> i;
	return Value(i).cloneData();
}

uint8_t* native_clock(int argc, uint8_t* args)
{
	return Value((double)clock() / CLOCKS_PER_SEC).cloneData();
}