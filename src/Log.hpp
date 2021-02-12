#pragma once
#include <iostream>
#include <string_view>

class Logger
{
public:
    enum LogColor { DEFAULT_COLOR = 39, BLACK = 30, RED, GREEN, YELLOW, BLUE, PURPLE, LIGHTBLUE, WHITE };

    static std::ostream& log(std::ostream& os, const std::string_view& str, LogColor color)
    {
        os << "\x1b[" << (int)color << "m" << str << "\x1b[39m";
        return os;
    }

    static std::ostream& setColor(std::ostream& os, LogColor color)
    {
       os << "\x1b[" << (int)color << "m";
       return os;
    }
};
