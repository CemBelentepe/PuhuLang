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
    Namespace<Var_T>* makeNamespace(std::string name);
    Namespace<Var_T>* getParent();
    Namespace<Var_T>* getChild(Token name);
    std::string getName();
    Namespace<Var_T>* getNamespace(const std::vector<std::string>& address);
    std::vector<std::string> getAddress();

    Var_T& addVariable(Var_T var);
    Var_T& getVariable(Token name);
    Var_T& getVariable(std::string name);
    Var_T& getVariable(const std::vector<std::string>& address, Token name);

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
Namespace<Var_T>* Namespace<Var_T>::makeNamespace(std::string name)
{
    auto it = childs.find(name);
    if (it == childs.end())
        return childs.insert({name, std::make_unique<Namespace<Var_T>>(this, name)}).first->second.get();
    else
        return it->second.get();
}

template <typename Var_T>
Namespace<Var_T>* Namespace<Var_T>::getParent()
{
    return parent;
}

template <typename Var_T>
Namespace<Var_T>* Namespace<Var_T>::getChild(Token name)
{
    auto it = childs.find(std::string(name.lexeme));
    if (it == childs.end())
    {
        throw Parser::TokenError("Namespace with name '" + std::string(name.lexeme) + "' is not defined inside the namespace '" + this->getName() + "'.", name);
    }
    return it->second.get();
}

template <typename Var_T>
std::string Namespace<Var_T>::getName()
{
    if (parent)
        return parent->getName() + name + "::";
    else
        return name + "::";
}

template <typename Var_T>
Namespace<Var_T>* Namespace<Var_T>::getNamespace(const std::vector<std::string>& address)
{
    if (address.size() == 0)
        return this;
    auto child = childs.find(address[0]);
    if (address.size() == 1)
    {
        return child->second.get();
    }
    else
    {
        std::vector<std::string> remaining;
        for (auto it = address.begin() + 1; it != address.end(); ++it)
            remaining.push_back(*it);
        return child->second->getNamespace(remaining);
    }
}

template <typename Var_T>
std::vector<std::string> Namespace<Var_T>::getAddress()
{
    if (parent)
    {
        auto vec = parent->getAddress();
        vec.push_back(name);
        return std::move(vec);
    }
    else
    {
        return {};
    }
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
            throw Parser::TokenError("Variable is not defined.", name);
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

template <typename Var_T>
Var_T& Namespace<Var_T>::getVariable(const std::vector<std::string>& address, Token name)
{
    if (address.size() > 0)
    {
        auto ns = childs.find(address[0]);
        if (ns == childs.end())
        {
            if (parent)
                return parent->getVariable(address, name);
            else if (address[0] == this->name)
            {
                std::vector<std::string> remaining;
                for (auto it = address.begin() + 1; it != address.end(); ++it)
                    remaining.push_back(*it);
                return this->getVariable(remaining, name);
            }
            else
            {
                std::string addr = "";
                for (auto& n : address)
                    addr += n + "::";
                throw Parser::TokenError("Variable with name '" + addr + std::string(name.lexeme) + "' is not defined inside the namespace '" + this->getName() + "'.", name);
            }
        }
        std::vector<std::string> remaining;
        for (auto it = address.begin() + 1; it != address.end(); ++it)
            remaining.push_back(*it);
        return ns->second->getVariable(remaining, name);
    }
    else
    {
        return getVariable(name);
    }
}
