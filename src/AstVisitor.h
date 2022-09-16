//
// Created by cembelentepe on 08/09/22.
//

#pragma once
#include "Expr.h"
#include "Stmt.h"

class ExprVisitorBase
{
public:
	virtual ~ExprVisitorBase() = default;

	virtual void visit(ExprBinary* expr) = 0;
	virtual void visit(ExprUnary* expr) = 0;
	virtual void visit(ExprLiteral* expr) = 0;
	virtual void visit(ExprVarGet* expr) = 0;
	virtual void visit(ExprVarSet* expr) = 0;
	virtual void visit(ExprCall* expr) = 0;
};

class StmtVisitorBase
{
public:
	virtual ~StmtVisitorBase() = default;

	virtual void visit(StmtExpr* stmt) = 0;
	virtual void visit(StmtBlock* stmt) = 0;
	virtual void visit(StmtIf* stmt) = 0;
	virtual void visit(StmtWhile* stmt) = 0;
	virtual void visit(StmtFor* stmt) = 0;
	virtual void visit(StmtReturn* stmt) = 0;

	virtual void visit(StmtDeclVar* stmt) = 0;
	virtual void visit(StmtDeclFunc* stmt) = 0;
};

template <typename T>
class ExprVisitor : public ExprVisitorBase
{
public:
	T result;
};

template <>
class ExprVisitor<void> : public ExprVisitorBase
{
};

template <typename T>
class StmtVisitor : public StmtVisitorBase
{
public:
	T result;
};

template <>
class StmtVisitor<void> : public StmtVisitorBase
{
};

