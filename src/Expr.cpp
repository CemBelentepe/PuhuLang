#include "Expr.hpp"
#include "AstVisitor.hpp"

template<typename T>
T Expr::accept(ExprVisitor<T>* visitor) 
{
    do_accept(visitor);
    return visitor->result;
}

template<>
void Expr::accept(ExprVisitor<void>* visitor) 
{
    do_accept(visitor);
}

template<>
std::shared_ptr<Type> Expr::accept(ExprVisitor<std::shared_ptr<Type>>* visitor) 
{
    do_accept(visitor);
    return visitor->result;
}

template<>
Value Expr::accept(ExprVisitor<Value>* visitor) 
{
    do_accept(visitor);
    return visitor->result;
}

void ExprLogic::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprBinary::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprUnary::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprCall::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprLiteral::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}
