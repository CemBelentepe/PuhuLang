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
};

class StmtVisitorBase
{
public:
	virtual ~StmtVisitorBase() = default;

	virtual void visit(StmtExpr* stmt) = 0;
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

