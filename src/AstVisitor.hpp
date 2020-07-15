#pragma once
#include "AST.h"

class AstVisitor
{
public:
    virtual void visit(ExprBinary* expr) = 0;
    virtual void visit(ExprCall* expr) = 0;
    virtual void visit(ExprCast* expr) = 0;
    virtual void visit(ExprLiteral* expr) = 0;
    virtual void visit(ExprUnary* expr) = 0;
    virtual void visit(ExprVariable* expr) = 0;

    virtual void visit(StmtBlock* stmt) = 0;
    virtual void visit(StmtExpr* stmt) = 0;
    virtual void visit(StmtFunc* stmt) = 0;
};