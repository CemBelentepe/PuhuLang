#include "AstDebugger.hpp"

AstDebugger::AstDebugger(std::unique_ptr<Expr>& root, std::ostream& os)
    : root(root), os(os)
{
}

void AstDebugger::debug()
{
    root->accept(this);
    os << std::endl;
}

void AstDebugger::visit(ExprBinary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
}

void AstDebugger::visit(ExprUnary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->rhs->accept(this);
    os << ")";
}

void AstDebugger::visit(ExprLiteral* expr)
{
    os << expr->value;
}
