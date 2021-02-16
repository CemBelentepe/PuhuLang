#include "AstDebugger.hpp"

AstDebugger::AstDebugger(std::unique_ptr<Expr>& root, std::ostream& os)
    : root(root), os(os), isShow(false)
{
}

void AstDebugger::debug()
{
    root->accept(this);
    os << std::endl;
}

void AstDebugger::showTypes(bool isShow) 
{
    this->isShow = isShow;
}

void AstDebugger::visit(ExprLogic* expr) 
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(isShow)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprBinary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(isShow)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprUnary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(isShow)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprLiteral* expr)
{
    os << expr->value;
    if(isShow)
        os << ": " << expr->type->toString();
}
