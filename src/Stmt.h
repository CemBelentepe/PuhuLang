//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include <utility>
#include "Token.h"
#include "Expr.h"

class StmtVisitorBase;
template<typename T>
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
		: expr(std::move(expr)), semicolon(std::move(semicolon))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};

class StmtBlock : public Stmt
{
public:
	std::vector<std::unique_ptr<Stmt>> stmts;
	Token openBrace;
	Token closeBrace;

	explicit StmtBlock(std::vector<std::unique_ptr<Stmt>> stmts, Token openBrace, Token closeBrace)
		: stmts(std::move(stmts)), openBrace(std::move(openBrace)), closeBrace(std::move(closeBrace))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};

class StmtIf : public Stmt
{
public:
	std::unique_ptr<Expr> cond;
	std::unique_ptr<Stmt> then;
	std::unique_ptr<Stmt> els;
	Token paren;

	explicit StmtIf(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then, std::unique_ptr<Stmt> els, Token paren)
		: cond(std::move(cond)), then(std::move(then)), els(std::move(els)), paren(std::move(paren))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};

class StmtWhile : public Stmt
{
public:
	std::unique_ptr<Expr> cond;
	std::unique_ptr<Stmt> body;
	Token paren;

	explicit StmtWhile(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body, Token paren)
		: cond(std::move(cond)), body(std::move(body)), paren(std::move(paren))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};

class StmtFor : public Stmt
{
public:
	std::unique_ptr<Expr> init; // Make decl or expr
	std::unique_ptr<Expr> cond;
	std::unique_ptr<Expr> fin;
	std::unique_ptr<Stmt> body;
	Token paren;

	explicit StmtFor(std::unique_ptr<Expr> init,
		std::unique_ptr<Expr> cond,
		std::unique_ptr<Expr> fin,
		std::unique_ptr<Stmt> body,
		Token paren)
		: init(std::move(init)), cond(std::move(cond)), fin(std::move(fin)), body(std::move(body)),
		  paren(std::move(paren))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};

class StmtReturn : public Stmt
{
public:
	std::unique_ptr<Expr> expr;
	Token ret;

	explicit StmtReturn(std::unique_ptr<Expr> expr, Token ret)
		: expr(std::move(expr)), ret(std::move(ret))
	{
	}

private:
	void doAccept(StmtVisitorBase* visitor) override;
};
