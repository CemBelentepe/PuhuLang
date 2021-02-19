#include "AstDebugger.hpp"

AstDebugger::AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os)
    : root(root), os(os), canShowType(false)
{
}

void AstDebugger::debug()
{
    for (auto& stmt : root)
    {
        stmt->accept(this);
    }
    std::cout << std::endl;
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
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprBinary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->lhs->accept(this);
    os << ", ";
    expr->rhs->accept(this);
    os << ")";
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprUnary* expr)
{
    os << "(" << expr->op.lexeme << ", ";
    expr->rhs->accept(this);
    os << ")";
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprLiteral* expr)
{
    os << expr->token.lexeme;
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprCall* expr)
{
    expr->callee->accept(this);
    os << "(";
    for (size_t i = 0; i < expr->args.size(); i++)
    {
        if (i != 0)
            os << ", ";
        expr->args[i]->accept(this);
    }
    os << ")";
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(StmtExpr* stmt)
{
    stmt->expr->accept(this);
    os << std::endl;
}

void AstDebugger::visit(DeclVar* stmt)
{
    os << "VAR " << stmt->name.lexeme;
    if (canShowType)
        os << " : " << stmt->type->toString();
    if (stmt->initter)
    {
        os << " = ";
        stmt->initter->accept(this);
    }
    os << std::endl;
}

void AstDebugger::visit(DeclFunc* stmt)
{
    os << "FUNC " << stmt->name.lexeme << "(";
    for (size_t i = 0; i < stmt->param_names.size(); i++)
    {
        if (i != 0)
            os << ", ";
        os << std::dynamic_pointer_cast<TypeFunction>(stmt->type)->param_types[i];
    }
    os << ") -> " << stmt->type->instrinsicType->toString() << " BEGIN\n";
    for (auto& s : stmt->body)
        s->accept(this);
    os << "END " << stmt->name.lexeme << std::endl;
}
