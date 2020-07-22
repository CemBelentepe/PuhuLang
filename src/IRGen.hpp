#pragma once

#include "AstVisitor.hpp"
#include "Enviroment.hpp"
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
    Enviroment* currentEnviroment;
    size_t currentLabel;

public:
    IRGen(std::vector<Stmt*>& root, std::unordered_map<std::string, Value*>& globals)
        : chunk(new IRChunk("_start")), root(root), globals(globals), currentEnviroment(new Enviroment(nullptr, 0)), currentLabel(0)
    {
        for (auto& val : globals)
            currentEnviroment->define(val.first, val.second->type);
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

    void beginScope(bool frameStart)
    {
        currentEnviroment = new Enviroment(currentEnviroment, frameStart ? 0 : currentEnviroment->currentPos);
    }

    void endScope()
    {
        std::vector<TypeTag> types = currentEnviroment->getEnvTypes();
        chunk->addCode(new InstPop(types));
        Enviroment* env = currentEnviroment->closing;
        delete currentEnviroment;
        currentEnviroment = env;
    }

    InstLabel* createLabel()
    {
        return new InstLabel(-1, currentLabel++, {});
    }

    void visit(ExprAssignment* expr)
    {
        expr->assignment->accept(this);
        Variable var = currentEnviroment->get(expr->name);
        if (var.depth == 0)
            chunk->addCode(new InstSetGlobal(expr->name.getString()));
        else
            chunk->addCode(new InstSetLocal(expr->name.getString(), var));
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
    void visit(ExprLogic* expr)
    {
        expr->left->accept(this);
        if (expr->op.type == TokenType::AND)
        {
            InstLabel* out = createLabel();
            chunk->addCode(new InstJump(-1, out, 1));
            chunk->addCode(new InstPop({TypeTag::BOOL}));
            expr->right->accept(this);
            chunk->addCode(out);
        }
        else
        {
            InstLabel* l1 = createLabel();
            InstLabel* l2 = createLabel();
            chunk->addCode(new InstJump(-1, l1, 1));
            chunk->addCode(new InstJump(-1, l2, 0));
            chunk->addCode(l1);
            chunk->addCode(new InstPop({TypeTag::BOOL}));
            expr->right->accept(this);
            chunk->addCode(l2);
        }
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
        Variable var = currentEnviroment->get(expr->name);
        if (var.depth == 0)
            chunk->addCode(new InstGetGlobal(expr->name.getString()));
        else
            chunk->addCode(new InstGetLocal(expr->name.getString(), currentEnviroment->get(expr->name)));
    }

    void visit(StmtBlock* stmt)
    {
        beginScope(false);

        for (auto& s : stmt->statements)
            s->accept(this);

        endScope();
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

        // currentEnviroment->define(stmt->name, func->type);

        beginScope(true);
        for (auto& s : stmt->args)
            currentEnviroment->define(s.first, s.second);

        for (auto& s : stmt->body->statements)
            s->accept(this);

        endScope();

        if (func->type.intrinsicType->tag == TypeTag::VOID)
            chunk->addCode(new InstReturn(TypeTag::VOID));

        chunks.push_back(func->irChunk);

        chunk = enclosing;
    }
    void visit(StmtVarDecleration* stmt)
    {
        if (stmt->initializer == nullptr)
            stmt->initializer = new ExprLiteral(new Value(0));
        stmt->initializer->accept(this);

        if (currentEnviroment->depth != 0)
            currentEnviroment->define(stmt->name, stmt->varType);
        if (currentEnviroment->depth == 0)
        {
            chunk->addCode(new InstSetGlobal(stmt->name.getString()));
            chunk->addCode(new InstPop({stmt->varType.tag}));
        }
        else
            chunk->addCode(new InstSetLocal(stmt->name.getString(), currentEnviroment->get(stmt->name)));
    }
    void visit(StmtReturn* stmt)
    {
        if (stmt->retVal != nullptr)
        {
            stmt->retVal->accept(this);
            chunk->addCode(new InstReturn(stmt->retVal->type.tag));
        }
        else
        {
            chunk->addCode(new InstReturn(TypeTag::VOID));
        }
    }
    void visit(StmtIf* stmt)
    {
        stmt->condition->accept(this);
        InstLabel* out = createLabel();
        if (stmt->els != nullptr)
        {
            InstLabel* elsLabel = createLabel();
            chunk->addCode(new InstJump(-1, elsLabel, 2));
            stmt->then->accept(this);
            chunk->addCode(new InstJump(-1, out, 0));
            chunk->addCode(elsLabel);
            stmt->els->accept(this);
        }
        else
        {
            chunk->addCode(new InstJump(-1, out, 2));
            stmt->then->accept(this);
        }
        chunk->addCode(out);
    }
    void visit(StmtFor* stmt)
    {
        beginScope(false);
        if (stmt->decl)
            stmt->decl->accept(this);

        InstLabel* start = createLabel();
        InstLabel* out = createLabel();
        chunk->addCode(start);
        stmt->cond->accept(this);
        chunk->addCode(new InstJump(-1, out, 2));
        stmt->loop->accept(this);
        if (stmt->inc)
            stmt->inc->accept(this);
        chunk->addCode(new InstJump(-1, start, 0));
        chunk->addCode(out);
        endScope();
    }
    void visit(StmtWhile* stmt)
    {
        InstLabel* start = createLabel();
        InstLabel* out = createLabel();
        chunk->addCode(start);
        stmt->condition->accept(this);
        chunk->addCode(new InstJump(-1, out, 2));
        stmt->loop->accept(this);
        chunk->addCode(new InstJump(-1, start, 0));
        chunk->addCode(out);
    }
};