#pragma once
#include "Token.hpp"
#include "Type.hpp"
#include "Value.hpp"

#include <memory>

class ExprVisitorBase;
template <typename T>
class ExprVisitor;

class Expr
{
public:
    enum class Instance
    {
        Binary,
        Unary,
        Literal
    };
    const Instance instance;
    std::shared_ptr<Type> type;

    Expr() = delete;
    explicit Expr(Instance instance, std::shared_ptr<Type> type)
        : instance(instance), type(type)
    {
    }

    virtual ~Expr() = default;

    template <typename T>
    T accept(ExprVisitor<T>* visitor);

    virtual void do_accept(ExprVisitorBase* visitor) = 0;
};

class ExprBinary : public Expr
{
public:
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    Token op;

    explicit ExprBinary(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, Token op)
        : Expr(Instance::Binary, Type::getNullType()), lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
    {
    }

    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprUnary : public Expr
{
public:
    std::unique_ptr<Expr> rhs;
    Token op;

    explicit ExprUnary(std::unique_ptr<Expr> rhs, Token op)
        : Expr(Instance::Unary, Type::getNullType()), rhs(std::move(rhs)), op(op)
    {
    }

    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprLiteral : public Expr
{
public:
    Value value;
    Token token;

    explicit ExprLiteral(Value value, Token token)
        : Expr(Instance::Literal, value.type), value(value), token(token)
    {
    }
    
    void do_accept(ExprVisitorBase* visitor) override;
};
