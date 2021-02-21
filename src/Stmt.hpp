#pragma once
#include "Expr.hpp"
#include "Token.hpp"
#include "Type.hpp"
#include "Value.hpp"

#include <memory>

class StmtVisitorBase;
template <typename T>
class StmtVisitor;

class Stmt
{
public:
    Stmt() = default;
    virtual ~Stmt() = default;

    template <typename T>
    T accept(StmtVisitor<T>* visitor);

private:
    virtual void do_accept(StmtVisitorBase* visitor) = 0;
};

class StmtExpr : public Stmt
{
public:
    std::unique_ptr<Expr> expr;
    Token semicolon;

    explicit StmtExpr(std::unique_ptr<Expr> expr, Token semicolon)
        : expr(std::move(expr)), semicolon(semicolon)
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class StmtBody : public Stmt
{
public:
    std::vector<std::unique_ptr<Stmt>> body;

    explicit StmtBody(std::vector<std::unique_ptr<Stmt>> body)
        : body(std::move(body))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class StmtReturn : public Stmt
{
public:
    Token retToken;
    std::unique_ptr<Expr> retExpr;

    explicit StmtReturn(Token retToken, std::unique_ptr<Expr> retExpr)
        : retToken(retToken), retExpr(std::move(retExpr))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class StmtIf : public Stmt
{
public:
    Token paren;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> then;
    std::unique_ptr<Stmt> els;

    explicit StmtIf(Token paren, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then, std::unique_ptr<Stmt> els)
        : paren(paren), cond(std::move(cond)), then(std::move(then)), els(std::move(els))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class StmtWhile : public Stmt
{
public:
    Token paren;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;

    explicit StmtWhile(Token paren, std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body)
        : paren(paren), cond(std::move(cond)), body(std::move(body))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class DeclVar : public Stmt
{
public:
    Token name;
    Token equal;
    std::shared_ptr<Type> type;
    std::unique_ptr<Expr> initter;

    explicit DeclVar(Token name, Token equal, std::shared_ptr<Type> type, std::unique_ptr<Expr> initter)
        : name(name), equal(equal), type(type), initter(std::move(initter))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};

class DeclFunc : public Stmt
{
public:
    Token name;
    std::shared_ptr<Type> type;
    std::unique_ptr<StmtBody> body;
    std::vector<Token> param_names;

    explicit DeclFunc(Token name, std::shared_ptr<Type> type, const std::vector<Token>& param_names, std::unique_ptr<StmtBody> body)
        : name(name), type(type), param_names(param_names), body(std::move(body))
    {
    }

private:
    void do_accept(StmtVisitorBase* visitor) override;
};
