#pragma once
#include "Value.hpp"
#include <string>
#include <vector>
#include <memory>

class Type;
class Interpreter;
class DeclFunc;
class Callable
{
public:
    Callable() = default;
    virtual ~Callable() = default;

    virtual Value call(Interpreter* interpreter, std::vector<Value> args) = 0;
    virtual std::vector<std::shared_ptr<Type>> arity() = 0;
    virtual std::string name() = 0;
    virtual std::shared_ptr<Type> getType() = 0;
    virtual std::vector<std::string> getNamespace() = 0;
};

class PuhuFunction : public Callable
{
public:
    DeclFunc* func;

    explicit PuhuFunction(DeclFunc* func);

    Value call(Interpreter* interpreter, std::vector<Value> args) override;
    std::vector<std::shared_ptr<Type>> arity() override;
    std::string name() override;
    std::shared_ptr<Type> getType() override;
    std::vector<std::string> getNamespace() override;
};
