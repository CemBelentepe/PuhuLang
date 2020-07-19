#pragma once

#include "Scanner.h"
#include "Value.hpp"

enum class ExprType
{
    Assignment,
    Binary,
    Call,
    Cast,
    Literal,
    Unary,
    Variable
};

class AstVisitor;

class Expr
{
public:
    const ExprType instance;
    Type type;

    Expr(ExprType instance, Type type)
        : instance(instance), type(type)
    {
    }

    virtual void accept(AstVisitor* visitor) = 0;
};

class ExprAssignment : public Expr
{
public:
    Token name;
    Expr* assignment;

    ExprAssignment(Token name, Expr* assignment)
        : Expr(ExprType::Assignment, TypeTag::NULL_TYPE), name(name), assignment(assignment)
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprBinary : public Expr
{
public:
    Expr* left;
    Expr* right;
    Token op;

    ExprBinary(Expr* left, Expr* right, Token op)
        : Expr(ExprType::Binary, TypeTag::NULL_TYPE), left(left), right(right), op(op)
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprCall : public Expr
{
public:
    Expr* callee;
    std::vector<Expr*> args;
    Token openParen;

    ExprCall(Expr* callee, std::vector<Expr*> args, Token openParen)
        : Expr(ExprType::Call, callee->type.intrinsicType), callee(callee), args(args), openParen(openParen)
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprCast : public Expr
{
public:
    Type from;
    Type to;
    Expr* expr;

    ExprCast(Type to, Expr* expr)
        : Expr(ExprType::Cast, to), from(TypeTag::NULL_TYPE), to(to), expr(expr)
    {
    }
    void accept(AstVisitor* visitor);
};

class ExprUnary : public Expr
{
public:
    Expr* expr;
    Token op;

    ExprUnary(Expr* expr, Token op)
        : Expr(ExprType::Unary, TypeTag::NULL_TYPE), expr(expr), op(op)
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprVariable : public Expr
{
public:
    Token name;

    ExprVariable(Token name)
        : Expr(ExprType::Variable, TypeTag::NULL_TYPE), name(name)
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprLiteral : public Expr
{
public:
    Value* val;

    ExprLiteral(Value* val)
        : Expr(ExprType::Literal, val->type), val(val)
    {
    }
    void accept(AstVisitor* visitor);
};

class Stmt
{
public:
    virtual void accept(AstVisitor* visitor) = 0;
};

class StmtExpr : public Stmt
{
public:
    Expr* expr;

    StmtExpr(Expr* expr)
        : expr(expr)
    {
    }

    void accept(AstVisitor* visitor);
};

class StmtBlock : public Stmt
{
public:
    std::vector<Stmt*> statements;

    StmtBlock(std::vector<Stmt*> statements)
        : statements(statements)
    {
    }

    void accept(AstVisitor* visitor);
};

class StmtFunc : public Stmt
{
public:
    Token name;
    StmtBlock* body;
    Type ret_type;
    std::vector<std::pair<Token, Type>> args;

    StmtFunc(Token name, StmtBlock* body, Type ret_type, std::vector<std::pair<Token, Type>> args)
        : name(name), ret_type(ret_type), body(body), args(args)
    {
    }

    void accept(AstVisitor* visitor);
};

class StmtVarDecleration : public Stmt
{
public:
    Type varType;
    Token name;
    Expr* initializer;

    StmtVarDecleration(Type varType, Token name, Expr* initializer)
        : varType(varType), name(name), initializer(initializer)
    {
    }

    void accept(AstVisitor* visitor);
};

class StmtReturn : public Stmt
{
public:
    Expr* retVal;

    StmtReturn(Expr* retVal)
        : retVal(retVal)
    {
    }

    void accept(AstVisitor* visitor);
};
