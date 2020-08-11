#pragma once

#include "Value.hpp"
#include <ctime>

std::vector<Data> native_print(int argc, Data* args)
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

    return {};
}

std::vector<Data> native_input(int argc, Data* args)
{
    std::string in;
    getline(std::cin, in);
    Data data;
    data.valString = new char[in.size() + 1];
    strcpy(data.valString, in.c_str());
    return {data};
}

std::vector<Data> native_inputInt(int argc, Data* args)
{
    Data i;
    std::cin >> i.valInt;
    return {i};
}

std::vector<Data> native_clock(int argc, Data* args)
{
    Data t;
    t.valDouble = (double)clock() / CLOCKS_PER_SEC;
    return {t};
}

std::vector<Data> native_rand(int argc, Data* args)
{
    Data t;
    t.valFloat = (float)rand() / (float)RAND_MAX;
    return {t};
}