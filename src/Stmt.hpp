#pragma once
#include "Token.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include "Expr.hpp"

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

    StmtExpr(std::unique_ptr<Expr> expr, Token semicolon)
        : expr(std::move(expr)), semicolon(semicolon)
    {}
private:
    void do_accept(StmtVisitorBase* visitor) override;
};
