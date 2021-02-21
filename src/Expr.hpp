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
        Logic,
        Binary,
        Unary,
        Call,
        Literal,
        VariableGet,
        VariableSet
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

private:
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

private:
    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprLogic : public Expr
{
public:
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    Token op;

    explicit ExprLogic(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, Token op)
        : Expr(Instance::Logic, std::make_unique<TypePrimitive>(TypePrimitive::PrimitiveTag::BOOL)), lhs(std::move(lhs)), rhs(std::move(rhs)), op(op)
    {
    }

private:
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

private:
    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprCall : public Expr
{
public:
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> args;
    Token paren;

    explicit ExprCall(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> args, Token paren)
        : Expr(Instance::Call, Type::getNullType()), callee(std::move(callee)), args(std::move(args)), paren(paren)
    {
    }

private:
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

private:
    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprVariableGet : public Expr
{
public:
    std::vector<std::string> address;
    Token name;

    explicit ExprVariableGet(const std::vector<std::string>& address, Token name)
        : Expr(Instance::VariableGet, Type::getNullType()), address(address), name(name)
    {
    }

private:
    void do_accept(ExprVisitorBase* visitor) override;
};

class ExprVariableSet : public Expr
{
public:
    std::vector<std::string> address;
    Token name;
    Token equal;
    std::unique_ptr<Expr> asgn;

    explicit ExprVariableSet(const std::vector<std::string>& address, Token name, Token equal, std::unique_ptr<Expr> asgn)
        : Expr(Instance::VariableSet, Type::getNullType()), address(address), name(name), equal(equal), asgn(std::move(asgn))
    {
    }

private:
    void do_accept(ExprVisitorBase* visitor) override;
};
