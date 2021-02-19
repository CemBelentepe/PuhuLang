#pragma once
#include "Token.hpp"
#include "Type.hpp"
#include "Parser.hpp"

#include <memory>
#include <string>
#include <unordered_map>

template <typename Var_T>
class Enviroment
{
public:
    explicit Enviroment(std::unique_ptr<Enviroment<Var_T>> parent = nullptr);
    ~Enviroment() = default;

    Enviroment* getParent();
    std::unique_ptr<Enviroment> returnToParent();

    Var_T& addVariable(Var_T var);
    Var_T& getVariable(Token name);

private:
    std::unique_ptr<Enviroment<Var_T>> parent;
    std::unordered_map<std::string, Var_T> variables;
};

template <typename Var_T>
Enviroment<Var_T>::Enviroment(std::unique_ptr<Enviroment> parent)
    : parent(std::move(parent))
{
}

template <typename Var_T>
Enviroment<Var_T>* Enviroment<Var_T>::getParent()
{
    return parent.get();
}

template <typename Var_T>
std::unique_ptr<Enviroment<Var_T>> Enviroment<Var_T>::returnToParent() 
{
    return std::move(parent);
}

template <typename Var_T>
Var_T& Enviroment<Var_T>::addVariable(Var_T var)
{
    std::string sName(var.name.lexeme);
    auto it = variables.find(sName);
    if (it == variables.end())
    {
        return variables.insert({sName, var}).first->second;
    }
    throw Parser::TokenError("Variable is already defined.", var.name);
}

template <typename Var_T>
Var_T& Enviroment<Var_T>::getVariable(Token name)
{
    std::string sName(name.lexeme);
    auto it = variables.find(sName);
    if (it == variables.end())
    {
        if (parent)
            return parent->getVariable(name);
        else
            throw Parser::TokenError("Variable is not defined defined.", name);
    }
    return it->second;
}
