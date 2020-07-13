#pragma once

#include "Scanner.h"
#include "AstVisitor.hpp"
#include "IRChunk.hpp"

class IRGen : public AstVisitor
{
private:
    IRChunk *chunk;
    Expr* root;
public:
    IRGen(Expr *expr)
        :chunk(new IRChunk()), root(expr)
    {    }

    IRChunk* generateIR()
    {
        root->accept(this);
        return this->chunk;
    }

    void visit(ExprBinary *expr)
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
    void visit(ExprCall *expr)
    {
        expr->callee->accept(this);
        for (auto &arg : expr->args)
            arg->accept(this);

        // TODO: add the call instruction
        // chunk->addCode(new InstCall());
    }
    void visit(ExprCast *expr)
    {
        expr->expr->accept(this);
        chunk->addCode(new InstCast(expr->from.tag, expr->to.tag));
    }
    void visit(ExprLiteral *expr)
    {
        size_t pos = chunk->addConstant(expr->val);
        chunk->addCode(new InstConst(pos));
    }
    void visit(ExprUnary *expr)
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
    void visit(ExprVariable *expr)
    {
        // TODO: Implement later
    }
};