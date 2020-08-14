#include "AST.h"
#include "AstVisitor.hpp"

void ExprArrGet::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}
void ExprArrSet::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprAssignment::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprBinary::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprCall::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprCast::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprLiteral::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprLogic::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprUnary::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprVariable::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprHeap::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprGetDeref::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprSetDeref::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprRef::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprTake::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprGet::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprSet::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void ExprAddr::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtBlock::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtFunc::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtExpr::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtVarDecleration::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtReturn::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtIf::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtWhile::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtFor::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

void StmtStruct::accept(AstVisitor* visitor)
{
    visitor->visit(this);
}

