#pragma once

#include "Value.hpp"
#include <ctime>

uint8_t* native_print(int argc, uint8_t* args)
{
	const char* str = *(char**)(args);
	size_t args_start = sizeof(char**);
	size_t i = 0;
	while(str[i] != 0)
	{
		const char& c = str[i++];
		if (c == '%')
		{
			switch (str[i])
			{
			case 'i':
				std::cout << *(int32_t*)(&args[args_start]);
				args_start += sizeof(int32_t);
				break;
			case 'f':
				std::cout << *(float*)(&args[args_start]);
				args_start += sizeof(float);
				break;
			case 'd':
				std::cout << *(double*)(&args[args_start]);
				args_start += sizeof(double);
				break;
			case 'c':
				std::cout << *(char*)(&args[args_start]);
				args_start += sizeof(char);
				break;
			case 's':
				std::cout << *(char**)(&args[args_start]);
				args_start += sizeof(char*);
				break;
			default:
				std::cout << args[i];
				break;
			}
			i++;
		}
		else
		{
			putchar(c);
		}
	}

	return nullptr;
}

/*
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
uint8_t* native_printlnInt(int argc, uint8_t* args)
{
	std::cout << *(int*)args << "\n";
	return nullptr;
}
*/

uint8_t* native_input(int argc, uint8_t* args)
{
	std::string in;
	getline(std::cin, in);
	return Value(in).cloneData();
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