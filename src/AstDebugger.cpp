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
    os << "(";
    expr->callee->accept(this);
    os << ")(";
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

void AstDebugger::visit(ExprVariableGet* expr)
{
    os << "(GET ";
    for(auto& t : expr->address)
    {
        os << t << "::";
    }
    os << expr->name.lexeme << ")";
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(ExprVariableSet* expr)
{
    os << "(SET ";
    for(auto& t : expr->address)
    {
        os << t << "::";
    }
    os << expr->name.lexeme << " = ";
    expr->asgn->accept(this);
    if (canShowType)
        os << ": " << expr->type->toString();
}

void AstDebugger::visit(StmtExpr* stmt)
{
    stmt->expr->accept(this);
    os << std::endl;
}

void AstDebugger::visit(StmtBody* stmt)
{
    os << "BEGIN\n";

    for (auto& s : stmt->body)
        s->accept(this);

    os << "END" << std::endl;
}

void AstDebugger::visit(StmtReturn* stmt)
{
    os << "RETURN ";
    if (stmt->retExpr)
        stmt->retExpr->accept(this);
    os << std::endl;
}

void AstDebugger::visit(StmtIf* stmt)
{
    os << "IF ";
    stmt->cond->accept(this);
    os << "\n";
    stmt->then->accept(this);
    if (stmt->els)
    {
        os << "ELSE ";
        stmt->els->accept(this);
    }
    os << std::endl;
}

void AstDebugger::visit(StmtWhile* stmt)
{
    os << "WHILE ";
    stmt->cond->accept(this);
    os << "\n";
    stmt->body->accept(this);
    os << std::endl;
}

void AstDebugger::visit(DeclVar* decl)
{
    os << "VAR " << decl->name.lexeme;
    if (canShowType)
        os << " : " << decl->type->toString();
    if (decl->initter)
    {
        os << " = ";
        decl->initter->accept(this);
    }
    os << std::endl;
}

void AstDebugger::visit(DeclFunc* decl)
{
    os << "FUNC " << decl->name.lexeme << "(";
    for (size_t i = 0; i < decl->param_names.size(); i++)
    {
        if (i != 0)
            os << ", ";
        os << std::dynamic_pointer_cast<TypeFunction>(decl->type)->param_types[i]->toString();
    }
    os << ") -> " << decl->type->instrinsicType->toString() << " ";
    decl->body->accept(this);
}

void AstDebugger::visit(DeclNamespace* decl) 
{
    os << "NAMESPACE " << decl->name.lexeme << "BEGIN\n";
    for(auto& stmt : decl->body)
    {
        stmt->accept(this);
    }
    os << "END" << std::endl;
}
