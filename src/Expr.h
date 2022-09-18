//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include <utility>
#include "Token.h"
#include "Type.h"

class ExprVisitorBase;
template<typename T>
class ExprVisitor;

class Expr
{
public:
	enum class Instance
	{
		Binary,
		Unary,
		Literal,
		VarGet,
		VarSet,
		Call,
		AddrOf,
		Deref,
	};
	const Instance instance;
	std::shared_ptr<Type> type;

	Expr() = delete;
	explicit Expr(Instance instance, std::shared_ptr<Type> type)
		: instance(instance), type(std::move(type))
	{
	}

	virtual ~Expr() = default;

	template<typename T>
	T accept(ExprVisitor<T>* visitor)
	{
		doAccept(visitor);
		return visitor->result;
	}

private:
	virtual void doAccept(ExprVisitorBase* visitor) = 0;
};

class ExprBinary : public Expr
{
public:
	Token op;
	std::unique_ptr<Expr> lhs;
	std::unique_ptr<Expr> rhs;

	explicit ExprBinary(Token op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
		: Expr(Instance::Binary, TypeFactory::getNull()), op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprUnary : public Expr
{
public:
	Token op;
	std::unique_ptr<Expr> rhs;

	explicit ExprUnary(std::unique_ptr<Expr> rhs, Token op)
		: Expr(Instance::Unary, TypeFactory::getNull()), op(std::move(op)), rhs(std::move(rhs))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprLiteral : public Expr
{
public:
	Token literal;

	explicit ExprLiteral(const Token& literal)
		: Expr(Instance::Literal, literal.getType()), literal(literal)
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprVarGet : public Expr
{
public:
	Token name;

	explicit ExprVarGet(Token name)
		: Expr(Instance::VarGet, TypeFactory::getNull()), name(std::move(name))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprVarSet : public Expr
{
public:
	Token name;
	Token op;
	std::unique_ptr<Expr> val;

	explicit ExprVarSet(Token name, Token op, std::unique_ptr<Expr> val)
		: Expr(Instance::VarSet, TypeFactory::getNull()), name(std::move(name)), op(std::move(op)), val(std::move(val))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprCall : public Expr
{
public:
	std::unique_ptr<Expr> callee;
	Token paren;
	std::vector<std::unique_ptr<Expr>> args;

	explicit ExprCall(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> args)
		: Expr(Instance::Call, TypeFactory::getNull()), callee(std::move(callee)), paren(std::move(paren)), args(std::move(args))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprAddrOf : public Expr
{
public:
	std::unique_ptr<Expr> lvalue;
	Token ampersand;

	explicit ExprAddrOf(std::unique_ptr<Expr> lvalue, Token ampersand)
		: Expr(Instance::AddrOf, TypeFactory::getNull()), lvalue(std::move(lvalue)), ampersand(std::move(ampersand))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprDeref : public Expr
{
public:
	std::unique_ptr<Expr> expr;
	Token star;

	explicit ExprDeref(std::unique_ptr<Expr> expr, Token star)
		: Expr(Instance::Deref, TypeFactory::getNull()), expr(std::move(expr)), star(std::move(star))
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};