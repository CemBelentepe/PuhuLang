//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include "Token.h"

class ExprVisitorBase;
template <typename T>
class ExprVisitor;

class Expr
{
public:
	enum class Instance
	{
		Binary,
		Unary,
		Literal,
	};
	const Instance instance;
	std::shared_ptr<Type> type;

	Expr() = delete;
	explicit Expr(Instance instance, std::shared_ptr<Type> type)
			: instance(instance), type(type)
	{
	}

	virtual ~Expr() = default;

	template <typename T>
	T accept(ExprVisitor<T>* visitor);

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
		: Expr(Instance::Binary, Type::getNullType()), op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
	{}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprUnary : public Expr
{
public:
	Token op;
	std::unique_ptr<Expr> rhs;

	explicit ExprUnary(std::unique_ptr<Expr> rhs, Token op)
			: Expr(Instance::Unary, Type::getNullType()), op(op), rhs(std::move(rhs))
	{}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};

class ExprLiteral : public Expr
{
public:
	Token literal;

	explicit ExprLiteral(Token literal)
			: Expr(Instance::Literal, literal.getType()), literal(literal)
	{
	}

private:
	void doAccept(ExprVisitorBase* visitor) override;
};