#include "NativeConsole.hpp"
#include <iostream>

Value NativePrint::call(Interpreter* interpreter, std::vector<Value> args) 
{
    std::cout << std::get<std::string>(args[0].data.data);
    return Value();
}

Value NativeInput::call(Interpreter* interpreter, std::vector<Value> args) 
{
    std::string input;
    std::getline(std::cin, input);
    return Value(input);
}
