#pragma once

#include "AstVisitor.hpp"
#include "IRChunk.hpp"
#include "Scanner.h"
#include <unordered_map>

class IRGen : public AstVisitor
{
private:
    IRChunk* chunk;
    std::vector<Stmt*>& root;
    std::unordered_map<std::string, Value*>& globals;
    std::vector<IRChunk*> chunks;

public:
    IRGen(std::vector<Stmt*>& root, std::unordered_map<std::string, Value*>& globals)
        : chunk(new IRChunk("entry")), root(root), globals(globals)
    {
    }

    std::vector<IRChunk*> generateIR()
    {
        chunks.push_back(chunk);
        for (auto& stmt : root)
            stmt->accept(this);

        chunk->addCode(new InstGetGlobal("main"));
        chunk->addCode(new InstCall({TypeTag::VOID}, TypeTag::FUNCTION));

        return chunks;
    }

    void visit(ExprBinary* expr)
    {
        expr->left->accept(this);
        expr->right->accept(this);

        switch (expr->op.type)
        {
        case TokenType::PLUS:
            chunk->addCode(new InstAdd(expr->left->type.tag));
            break;
        case TokenType::MINUS:
            chunk->addCode(new InstSub(expr->left->type.tag));
            break;
        case TokenType::STAR:
            chunk->addCode(new InstMul(expr->left->type.tag));
            break;
        case TokenType::SLASH:
            chunk->addCode(new InstDiv(expr->left->type.tag));
            break;
        case TokenType::MODULUS:
            chunk->addCode(new InstMod());
            break;
        case TokenType::BIT_OR:
        case TokenType::BIT_XOR:
        case TokenType::BIT_AND:
        case TokenType::BITSHIFT_LEFT:
        case TokenType::BITSHIFT_RIGHT:
            chunk->addCode(new InstBit(expr->op.type));
            break;
        case TokenType::LESS:
            chunk->addCode(new InstLess(expr->left->type.tag));
            break;
        case TokenType::LESS_EQUAL:
            chunk->addCode(new InstLte(expr->left->type.tag));
            break;
        case TokenType::GREAT:
            chunk->addCode(new InstGreat(expr->left->type.tag));
            break;
        case TokenType::GREAT_EQUAL:
            chunk->addCode(new InstGte(expr->left->type.tag));
            break;
        case TokenType::EQUAL_EQUAL:
            chunk->addCode(new InstEq(expr->left->type.tag));
            break;
        case TokenType::BANG_EQUAL:
            chunk->addCode(new InstNeq(expr->left->type.tag));
            break;
        }
    }
    void visit(ExprCall* expr)
    {
        std::vector<TypeTag> args;
        for (auto& arg : expr->args)
        {
            arg->accept(this);
            args.push_back(arg->type.tag);
        }

        expr->callee->accept(this);
        chunk->addCode(new InstCall(args, expr->callee->type.tag));
    }
    void visit(ExprCast* expr)
    {
        expr->expr->accept(this);
        chunk->addCode(new InstCast(expr->from.tag, expr->to.tag));
    }
    void visit(ExprLiteral* expr)
    {
        size_t pos = chunk->addConstant(expr->val);
        chunk->addCode(new InstConst(pos));
    }
    void visit(ExprUnary* expr)
    {
        expr->expr->accept(this);

        switch (expr->op.type)
        {
        case TokenType::BANG:
            chunk->addCode(new InstNot());
            break;
        case TokenType::TILDE:
            chunk->addCode(new InstBit(TokenType::TILDE));
        }
    }
    void visit(ExprVariable* expr)
    {
        // TODO: Implement later
        chunk->addCode(new InstGetGlobal(expr->name.getString()));
    }

    void visit(StmtBlock* stmt)
    {
        for (auto& s : stmt->statements)
            s->accept(this);

        // TODO: Pop enviroment
    }
    void visit(StmtExpr* stmt)
    {
        stmt->expr->accept(this);
        chunk->addCode(new InstPop({stmt->expr->type.tag}));
    }
    void visit(StmtFunc* stmt)
    {
        IRChunk* enclosing = chunk;
        chunk = new IRChunk(stmt->name.getString());
        
        FuncValue* func = (FuncValue*)globals[stmt->name.getString()];
        func->irChunk = chunk;
        func->data.valChunk = func->irChunk->chunk;

        // TODO: ENVIROMENT
        stmt->body->accept(this);

        if(func->type == TypeTag::VOID)
            chunk->addCode(new InstReturn(TypeTag::VOID));

        chunks.push_back(func->irChunk);

        chunk = enclosing;
    }
};