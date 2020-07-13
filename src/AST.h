#pragma once

#include "Scanner.h"
#include "Value.h"

class AstVisitor;

class Expr
{
public:
	Type type;
    
	Expr(Type type)
		:type(type)
	{}

	virtual void accept(AstVisitor* visitor) = 0;
};

class ExprBinary : public Expr
{
public:
	Expr* left;
	Expr* right;
	Token op;

	ExprBinary(Expr* left, Expr* right, Token op)
		:Expr(TypeTag::NULL_TYPE), left(left), right(right), op(op)
	{}

	void accept(AstVisitor* visitor);
};

class ExprCall : public Expr
{
public:
	Expr* callee;
	std::vector<Expr*> args;
	Token openParen;

	ExprCall(Expr* callee, std::vector<Expr*> args, Token openParen)
		:Expr(callee->type.intrinsicType), callee(callee), args(args), openParen(openParen)
	{}

	void accept(AstVisitor* visitor);
};

class ExprCast : public Expr
{
public:
	Type from;
	Type to;
	Expr* expr;

	ExprCast(Type to, Expr* expr)
		:Expr(to), from(TypeTag::NULL_TYPE), to(to), expr(expr)
	{}
	void accept(AstVisitor* visitor);
};

class ExprUnary : public Expr
{
public:
	Expr* expr;
	Token op;

	ExprUnary(Expr* expr, Token op)
		:Expr(TypeTag::NULL_TYPE), expr(expr), op(op)
	{}

	void accept(AstVisitor* visitor);
};

class ExprVariable : public Expr
{
public:
	Token name;

	ExprVariable(Token name)
		:Expr(TypeTag::NULL_TYPE), name(name)
	{}

	void accept(AstVisitor* visitor);
};

class ExprLiteral : public Expr
{
public:
	Value* val;

	ExprLiteral(Value* val)
		:Expr(val->type), val(val)
	{}
	void accept(AstVisitor* visitor);
};