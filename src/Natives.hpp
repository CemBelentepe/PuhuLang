#pragma once

#include "Value.hpp"
#include <ctime>

Data native_print(int argc, Data* args)
{
    const char* str = args[0].valString;
    size_t args_start = 1;
    size_t i = 0;
    while (str[i] != 0)
    {
        const char& c = str[i++];
        if (c == '%')
        {
            switch (str[i])
            {
            case 'i':
                std::cout << args[args_start].valInt;
                break;
            case 'f':
                std::cout << args[args_start].valFloat;
                break;
            case 'd':
                std::cout << args[args_start].valDouble;
                break;
            case 'c':
                std::cout << args[args_start].valChar;
                break;
            case 's':
                std::cout << args[args_start].valString;
                break;
            default:
                std::cout << args[args_start].valChunk;
                break;
            }
            args_start++;
            i++;
        }
        else
        {
            putchar(c);
        }
    }

    return Data{0};
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

Data native_input(int argc, Data* args)
{
    std::string in;
    getline(std::cin, in);
    Data data;
    data.valString = new char[in.size() + 1];
    strcpy(data.valString, in.c_str());
    return data;
}

Data native_inputInt(int argc, Data* args)
{
    Data i;
    std::cin >> i.valInt;
    return i;
}

Data native_clock(int argc, Data* args)
{
    Data t;
    t.valDouble = (double)clock() / CLOCKS_PER_SEC;
    ;
    return t;
}