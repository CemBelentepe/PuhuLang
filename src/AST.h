#pragma once

#include "Scanner.h"
#include "Value.hpp"

#include <unordered_map>

enum class ExprType
{
    Assignment,
    ArrGet,
    ArrSet,
    Binary,
    Call,
    Cast,
    Literal,
    Logic,
    Unary,
    Variable,
    Heap,
    GetDeref,
    SetDeref,
    Ref,
    Take,
    Get,
    Set
};

class AstVisitor;

class Expr
{
public:
    const ExprType instance;
    std::shared_ptr<Type> type;

    Expr(ExprType instance, std::shared_ptr<Type> type)
        : instance(instance), type(type)
    {
    }

    virtual ~Expr() {}

    virtual void accept(AstVisitor* visitor) = 0;
};

class ExprArrGet : public Expr
{
public:
    Expr* callee;
    Expr* index;
    Token bracket;

    ExprArrGet(Expr* callee, Expr* index, Token bracket)
        : Expr(ExprType::ArrGet, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), index(index), bracket(bracket)
    {
    }

    ~ExprArrGet()
    {
        delete callee;
        delete index;
    }

    void accept(AstVisitor* visitor);
};

class ExprArrSet : public Expr
{
public:
    Expr* callee;
    Expr* index;
    Expr* assignment;
    Token bracket;

    ExprArrSet(Expr* callee, Expr* index, Expr* assignment, Token bracket)
        : Expr(ExprType::ArrSet, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), index(index), assignment(assignment), bracket(bracket)
    {
    }

    ~ExprArrSet()
    {
        delete callee;
        delete index;
    }

    void accept(AstVisitor* visitor);
};

class ExprAssignment : public Expr
{
public:
    Token name;
    Expr* assignment;

    ExprAssignment(Token name, Expr* assignment)
        : Expr(ExprType::Assignment, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), name(name), assignment(assignment)
    {
    }

    ~ExprAssignment()
    {
        delete assignment;
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
        : Expr(ExprType::Binary, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), left(left), right(right), op(op)
    {
    }

    ~ExprBinary()
    {
        delete left;
        delete right;
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
        : Expr(ExprType::Call, callee->type->intrinsicType), callee(callee), args(args), openParen(openParen)
    {
    }

    ~ExprCall()
    {
        delete callee;
        for (auto& arg : args)
            delete arg;
    }

    void accept(AstVisitor* visitor);
};

class ExprCast : public Expr
{
public:
    std::shared_ptr<Type> from;
    std::shared_ptr<Type> to;
    Expr* expr;

    ExprCast(std::shared_ptr<Type> to, Expr* expr)
        : Expr(ExprType::Cast, to), from(std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), to(to), expr(expr)
    {
    }

    ~ExprCast()
    {
        delete expr;
    }

    void accept(AstVisitor* visitor);
};

class ExprUnary : public Expr
{
public:
    Expr* expr;
    Token op;

    ExprUnary(Expr* expr, Token op)
        : Expr(ExprType::Unary, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), expr(expr), op(op)
    {
    }

    ~ExprUnary()
    {
        delete expr;
    }

    void accept(AstVisitor* visitor);
};

class ExprVariable : public Expr
{
public:
    Token name;

    ExprVariable(Token name)
        : Expr(ExprType::Variable, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), name(name)
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

class ExprLogic : public Expr
{
public:
    Expr* left;
    Expr* right;
    Token op;

    ExprLogic(Expr* left, Expr* right, Token op)
        : Expr(ExprType::Logic, std::make_shared<TypePrimitive>(TypeTag::BOOL)), left(left), right(right), op(op)
    {
    }

    ~ExprLogic()
    {
        delete left;
        delete right;
    }

    void accept(AstVisitor* visitor);
};

class ExprHeap : public Expr
{
public:
    std::shared_ptr<Type> constructType;
    Token token;

    ExprHeap(std::shared_ptr<Type> constructType, Token token)
        : Expr(ExprType::Heap, std::make_shared<TypePointer>(true, constructType)), constructType(constructType), token(token)
    {
    }

    ~ExprHeap()
    {
    }

    void accept(AstVisitor* visitor);
};

class ExprGetDeref : public Expr
{
public:
    Expr* callee;
    Token token;

    ExprGetDeref(Expr* callee, Token token)
        : Expr(ExprType::GetDeref, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), token(token) {}

    ~ExprGetDeref()
    {
        delete callee;
    }

    void accept(AstVisitor* visitor);
};

class ExprSetDeref : public Expr
{
public:
    Expr* callee;
    Expr* asgn;
    Token token;
    Token equal;

    ExprSetDeref(Expr* callee, Expr* asgn, Token token, Token equal)
        : Expr(ExprType::SetDeref, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), asgn(asgn), token(token), equal(equal) {}

    ~ExprSetDeref()
    {
        delete callee;
    }

    void accept(AstVisitor* visitor);
};

class ExprRef : public Expr
{
public:
    Expr* callee;
    Token token;

    ExprRef(Expr* callee, Token token)
        : Expr(ExprType::Ref, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), token(token)
    {
    }

    ~ExprRef()
    {
        delete callee;
    }

    void accept(AstVisitor* visitor);
};

class ExprTake : public Expr
{
public:
    Expr* source;
    Token token;

    ExprTake(Expr* source, Token token)
        : Expr(ExprType::Ref, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), source(source), token(token) {}

    ~ExprTake()
    {
        delete source;
    }

    void accept(AstVisitor* visitor);
};

class ExprGet : public Expr
{
public:
    Expr* callee;
    Token get;

    ExprGet(Expr* callee, Token get)
        : Expr(ExprType::Get, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), get(get) {}

    ~ExprGet()
    {
        delete callee;
    }

    void accept(AstVisitor* visitor);
};

class ExprSet : public Expr
{
public:
    Expr* callee;
    Expr* asgn;
    Token get;

    ExprSet(Expr* callee, Expr* asgn, Token get)
        : Expr(ExprType::Set, std::make_shared<TypePrimitive>(TypeTag::NULL_TYPE)), callee(callee), asgn(asgn), get(get) {}

    ~ExprSet()
    {
        delete callee;
        delete asgn;
    }

    void accept(AstVisitor* visitor);
};

class Stmt
{
public:
    virtual ~Stmt() {}
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

    ~StmtExpr()
    {
        delete expr;
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

    ~StmtBlock()
    {
        for (auto& stmt : statements)
            delete stmt;
    }

    void accept(AstVisitor* visitor);
};

class StmtFunc : public Stmt
{
public:
    Token name;
    StmtBlock* body;
    std::shared_ptr<TypeFunction> func_type;
    std::vector<Token> args;

    StmtFunc(Token name, StmtBlock* body, std::shared_ptr<TypeFunction> func_type, std::vector<Token> args)
        : name(name), func_type(func_type), body(body), args(args)
    {
    }

    ~StmtFunc()
    {
        delete body;
    }

    void accept(AstVisitor* visitor);
};

class StmtVarDecleration : public Stmt
{
public:
    std::shared_ptr<Type> varType;
    Token name;
    Expr* initializer;

    StmtVarDecleration(std::shared_ptr<Type> varType, Token name, Expr* initializer)
        : varType(varType), name(name), initializer(initializer)
    {
    }

    ~StmtVarDecleration()
    {
        if (initializer)
            delete initializer;
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

    ~StmtReturn()
    {
        delete retVal;
    }

    void accept(AstVisitor* visitor);
};

class StmtIf : public Stmt
{
public:
    Expr* condition;
    Stmt* then;
    Stmt* els;
    Token paren;

    StmtIf(Expr* condition, Stmt* then, Stmt* els, Token paren)
        : condition(condition), then(then), els(els), paren(paren)
    {
    }

    ~StmtIf()
    {
        delete condition;
        delete then;
        if (els)
            delete els;
    }

    void accept(AstVisitor* visitor);
};

class StmtFor : public Stmt
{
public:
    Stmt* decl;
    Expr* cond;
    Expr* inc;
    Stmt* loop;
    Token paren;

    StmtFor(Stmt* decl, Expr* cond, Expr* inc, Stmt* loop, Token paren)
        : decl(decl), cond(cond), inc(inc), loop(loop), paren(paren)
    {
    }

    ~StmtFor()
    {
        if (decl)
            delete decl;
        delete cond;
        if (inc)
            delete inc;
        delete loop;
    }

    void accept(AstVisitor* visitor);
};

class StmtWhile : public Stmt
{
public:
    Expr* condition;
    Stmt* loop;
    Token paren;

    StmtWhile(Expr* condition, Stmt* loop, Token paren)
        : condition(condition), loop(loop), paren(paren)
    {
    }

    ~StmtWhile()
    {
        delete condition;
        delete loop;
    }

    void accept(AstVisitor* visitor);
};

class StmtClass : public Stmt
{
public:
    std::shared_ptr<TypeClass> type;
    std::unordered_map<std::string, StmtFunc*> methodes;
    Token token;

    StmtClass(std::shared_ptr<TypeClass> type, Token token)
        : type(type), token(token)
    {
    }

    ~StmtClass()
    {
        for (auto& f : methodes)
        {
            delete f.second;
        }
    }

    void accept(AstVisitor* visitor);
};