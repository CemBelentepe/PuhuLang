#pragma once
#include "AST.h"

class AstVisitor
{
public:
    virtual void visit(ExprArrGet* expr) = 0;
    virtual void visit(ExprArrSet* expr) = 0;
    virtual void visit(ExprAssignment* expr) = 0;
    virtual void visit(ExprBinary* expr) = 0;
    virtual void visit(ExprCall* expr) = 0;
    virtual void visit(ExprCast* expr) = 0;
    virtual void visit(ExprLiteral* expr) = 0;
    virtual void visit(ExprLogic* expr) = 0;
    virtual void visit(ExprUnary* expr) = 0;
    virtual void visit(ExprVariable* expr) = 0;
    virtual void visit(ExprHeap* expr) = 0;
    virtual void visit(ExprGetDeref* expr) = 0;
    virtual void visit(ExprSetDeref* expr) = 0;
    virtual void visit(ExprRef* expr) = 0;
    virtual void visit(ExprTake* expr) = 0;
    virtual void visit(ExprGet* expr) = 0;
    virtual void visit(ExprSet* expr) = 0;
    virtual void visit(ExprAddr* expr) = 0;

    virtual void visit(StmtBlock* stmt) = 0;
    virtual void visit(StmtExpr* stmt) = 0;
    virtual void visit(StmtFunc* stmt) = 0;
    virtual void visit(StmtVarDecleration* stmt) = 0;
    virtual void visit(StmtReturn* stmt) = 0;
    virtual void visit(StmtIf* stmt) = 0;
    virtual void visit(StmtFor* stmt) = 0;
    virtual void visit(StmtWhile* stmt) = 0;
    virtual void visit(StmtStruct* stmt) = 0;
};