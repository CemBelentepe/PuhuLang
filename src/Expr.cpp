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

void ExprBinary::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprUnary::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}

void ExprLiteral::do_accept(ExprVisitorBase* visitor) 
{
    visitor->visit(this);
}
