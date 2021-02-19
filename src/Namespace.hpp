#pragma once
#include "Parser.hpp"
#include "Token.hpp"
#include "Type.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

template <typename Var_T>
class Namespace
{
public:
    explicit Namespace(Namespace<Var_T>* parent = nullptr, const std::string& name = "");
    template <typename Var_U>
    explicit Namespace(const std::unique_ptr<Namespace<Var_U>>& other, Namespace<Var_T>* parent = nullptr);
    ~Namespace() = default;

    Namespace<Var_T>* makeNamespace(Token name);
    Namespace<Var_T>* getParent();

    Var_T& addVariable(Var_T var);
    Var_T& getVariable(Token name);
    Var_T& getVariable(std::string name);

public:
    std::string name;
    Namespace<Var_T>* parent;
    std::unordered_map<std::string, std::unique_ptr<Namespace<Var_T>>> childs;
    std::unordered_map<std::string, Var_T> variables;
};

template <typename Var_T>
Namespace<Var_T>::Namespace(Namespace<Var_T>* parent, const std::string& name)
    : parent(parent), name(name)
{
}

template <typename Var_T>
template <typename Var_U>
Namespace<Var_T>::Namespace(const std::unique_ptr<Namespace<Var_U>>& other, Namespace<Var_T>* parent)
    : name(other->name), parent(parent)
{
    for (const auto& c : other->childs)
    {
        childs.insert({c.first, std::make_unique<Namespace<Var_T>>(c.second, this)});
    }

    for (const auto& v : other->variables)
    {
        variables.insert({v.first, Var_T(v.second)});
    }
}

template <typename Var_T>
Namespace<Var_T>* Namespace<Var_T>::makeNamespace(Token name)
{
    std::string sName(name.lexeme);
    auto it = childs.find(sName);
    if (it == childs.end())
        return childs.insert({sName, std::make_unique<Namespace<Var_T>>(this, sName)}).first->second.get();
    else
        return it->second.get();
}

template <typename Var_T>
Namespace<Var_T>* Namespace<Var_T>::getParent()
{
    return parent;
}

template <typename Var_T>
Var_T& Namespace<Var_T>::addVariable(Var_T var)
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
Var_T& Namespace<Var_T>::getVariable(Token name)
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

template <typename Var_T>
Var_T& Namespace<Var_T>::getVariable(std::string name)
{
    auto it = variables.find(name);
    if (it == variables.end())
    {
        if (parent)
            return parent->getVariable(name);
        else
        {
            throw "Variable with name '" + name + "' is not defined";
        }
    }
    return it->second;
}
