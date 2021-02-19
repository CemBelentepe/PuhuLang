#pragma once
#include "Token.hpp"

#include <memory>

class Type;
class Variable
{
public:
    Token name;
    std::shared_ptr<Type> type;
    bool initialized;

    Variable() = default;
    explicit Variable(const Token& name, const std::shared_ptr<Type>& type, bool initialized);
};