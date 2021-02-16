#include "AstDebugger.hpp"

AstDebugger::AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os)
    : root(root), os(os), canShowType(false)
{
}

void AstDebugger::debug()
{
    for(auto& stmt : root)
    {
        stmt->accept(this);
    }
}

void AstDebugger::showTypes(bool isShow) 
{
    this->canShowType = isShow;
}

void AstDebugger::visit(ExprLogic* expr) 
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprBinary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprUnary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->rhs->accept(this);
    os << ")";
    if(canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprLiteral* expr)
{
    os << expr->token.lexeme;
    if(canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(StmtExpr* stmt) 
{
    stmt->expr->accept(this);
    os << '\n';
}
