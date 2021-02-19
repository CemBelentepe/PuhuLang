#pragma once
#include "Type.hpp"
#include "Stmt.hpp"
#include "Interpreter.hpp"

#include <vector>

class Callable
{
public:
    Callable() = default;
    virtual ~Callable() = default;

    virtual Value call(Interpreter* interpreter, std::vector<Value> args) = 0;
    virtual std::vector<std::shared_ptr<Type>> arity() = 0;
    virtual std::string name() = 0;
    virtual std::shared_ptr<Type> getType() = 0;
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
};
