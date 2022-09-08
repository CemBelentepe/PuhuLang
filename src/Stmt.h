//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include "Token.h"
#include "Expr.h"

class StmtVisitorBase;
template <typename T>
class StmtVisitor;

class Stmt
{
public:
	Stmt() = default;
	virtual ~Stmt() = default;

	template<typename T>
	T accept(StmtVisitor<T>* visitor);
private:
	virtual void doAccept(StmtVisitorBase* visitor) = 0;
};

class StmtExpr : public Stmt
{
public:
	std::unique_ptr<Expr> expr;
	Token semicolon;

	explicit StmtExpr(std::unique_ptr<Expr> expr, Token semicolon)
		: expr(std::move(expr)), semicolon(semicolon)
	{}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};
