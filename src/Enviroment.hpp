#pragma once

#include "Scanner.h"
#include "Value.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Variable
{
public:
    int depth;
    int position;
    Type type;

    Variable()
        : depth(0), position(0) {}

    Variable(int depth, int position, Type type)
        : depth(depth), position(position), type(type) {}
};

class Enviroment
{
public:
    std::unordered_map<std::string, Variable> values;
    Enviroment* closing;
    int depth;
    int currentPos;

    Enviroment(Enviroment* closing, int currentPos)
        : closing(closing), currentPos(currentPos)
    {
        depth = closing ? closing->depth + 1 : 0;
    }

    Variable get(Token& name)
    {
        if (values.find(name.getString()) != values.end())
            return values[name.getString()];

        else if (closing != nullptr)
            return closing->get(name);

        else
            std::cout << "[ERROR] Unknown variable '" << name.getString() << "' at line " << name.line << "\n";

        return Variable();
    }

    void define(Token& name, Type type)
    {
        if (values.find(name.getString()) == values.end())
        {
            values.insert({name.getString(), Variable(depth, currentPos, type)});
            currentPos += type.getSize();
        }
        else
            std::cout << "[ERROR] Variable '" << name.getString() << "' has already defined at this scope at line " << name.line << "\n";
    }

    void define(std::string name, Type type)
    {
        if (values.find(name) == values.end())
        {
            values.insert({name, Variable(depth, currentPos, type)});
            currentPos += type.getSize();
        }
        else
            std::cout << "[ERROR] Variable '" << name << "' has already defined at this scope\n";
    }

    std::vector<TypeTag> getEnvTypes()
    {
        std::vector<TypeTag> types;
        for (auto& var : values)
            types.push_back(var.second.type.tag);
        return types;
    }
};