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
    bool cont;
    IRGen(std::vector<Stmt*>& root, std::unordered_map<std::string, Value*>& globals)
        : chunk(new IRChunk("_start")), root(root), globals(globals), currentEnviroment(new Enviroment(nullptr, 0)), currentLabel(0), cont(true)
    {
        for (auto& val : globals)
            currentEnviroment->define(val.first, val.second->type, true); // TODO: fix that thing
    }

    std::vector<std::shared_ptr<Type>> makePrimTypeList(std::vector<TypeTag> tags)
    {
        std::vector<std::shared_ptr<Type>> types;
        for (auto& tag : tags)
            types.push_back(std::make_shared<TypePrimitive>(tag));
        return types;
    }

    std::vector<IRChunk*> generateIR()
    {
        chunks.push_back(chunk);
        for (auto& stmt : root)
            stmt->accept(this);

        chunk->addCode(new InstGetGlobal("main", globals["main"]->type));
        chunk->addCode(new InstCall(makePrimTypeList({TypeTag::VOID}), TypeTag::FUNCTION));

        return chunks;
    }

    void beginScope(bool frameStart)
    {
        currentEnviroment = new Enviroment(currentEnviroment, frameStart ? 0 : currentEnviroment->currentPos);
    }

    void endScope()
    {
        for (auto& var : currentEnviroment->values)
        {
            if (var.second.type->tag == TypeTag::POINTER && ((TypePointer*)(var.second.type.get()))->is_owner)
            {
                chunk->addCode(new InstGetLocal(var.first, var.second, var.second.type));
                chunk->addCode(new InstFree());
            }
        }
        auto types = currentEnviroment->getEnvTypes();
        chunk->addCode(new InstPop(types));
        Enviroment* env = currentEnviroment->closing;
        delete currentEnviroment;
        currentEnviroment = env;
    }

    InstLabel* createLabel()
    {
        return new InstLabel(-1, currentLabel++, {});
    }

    void visit(ExprArrGet* expr)
    {
        if (expr->type->getSize() != 1)
            expr->index = new ExprBinary(expr->index, new ExprLiteral(new Value((int)expr->type->getSize())), Token(TokenType::STAR, expr->bracket.line, expr->bracket.start, expr->bracket.length));

        while (expr->callee->instance == ExprType::ArrGet)
        {
            ExprArrGet* child = (ExprArrGet*)expr->callee;
            expr->callee = child->callee;

            Token plus(TokenType::PLUS, expr->bracket.line, expr->bracket.start, expr->bracket.length);
            child->index = new ExprBinary(child->index, new ExprLiteral(new Value((int)child->type->getSize())), Token(TokenType::STAR, child->bracket.line, child->bracket.start, child->bracket.length));
            expr->index = new ExprBinary(expr->index, child->index, plus);
        }

        if (expr->callee->instance == ExprType::Variable)
        {
            ExprVariable* exprVar = (ExprVariable*)(expr->callee);
            Variable var = currentEnviroment->get(exprVar->name);

            expr->index->accept(this);

            if (var.depth == 0)
                chunk->addCode(new InstGetGlobal(exprVar->name.getString(), expr->type, true));
            else
                chunk->addCode(new InstGetLocal(exprVar->name.getString(), currentEnviroment->get(exprVar->name), expr->type, true));
        }
        else if (expr->callee->instance == ExprType::Get)
        {
            ExprGet* get = (ExprGet*)expr->callee;

            if (get->callee->instance == ExprType::Variable)
            {
                expr->index->accept(this);
                TypeStruct* type = (TypeStruct*)get->callee->type.get();
                std::string getName = get->get.getString();
                ExprVariable* exprVar = (ExprVariable*)get->callee;
                Variable var = currentEnviroment->get(exprVar->name);
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
                chunk->addCode(new InstGetLocal(exprVar->name.getString(), var, expr->type, true));
            }
            else if (get->callee->instance == ExprType::GetDeref)
            {
                ExprGetDeref* exprGet = (ExprGetDeref*)get->callee;
                exprGet->callee->accept(this);
                expr->index->accept(this);
                TypeStruct* type = (TypeStruct*)exprGet->callee->type->intrinsicType.get();
                std::string getName = get->get.getString();
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
                chunk->addCode(new InstGetDerefOff(expr->type));
            }
        }

        // TODO: 2D Arrays
    }

    void visit(ExprArrSet* expr)
    {
        if (expr->type->getSize() != 1)
            expr->index = new ExprBinary(expr->index, new ExprLiteral(new Value((int)expr->type->getSize())), Token(TokenType::STAR, expr->bracket.line, expr->bracket.start, expr->bracket.length));

        while (expr->callee->instance == ExprType::ArrGet)
        {
            ExprArrGet* child = (ExprArrGet*)expr->callee;
            expr->callee = child->callee;

            Token plus(TokenType::PLUS, expr->bracket.line, expr->bracket.start, expr->bracket.length);
            child->index = new ExprBinary(child->index, new ExprLiteral(new Value((int)child->type->getSize())), Token(TokenType::STAR, child->bracket.line, child->bracket.start, child->bracket.length));
            expr->index = new ExprBinary(expr->index, child->index, plus);
        }

        expr->assignment->accept(this);

        if (expr->callee->instance == ExprType::Variable)
        {
            ExprVariable* exprVar = (ExprVariable*)(expr->callee);
            Variable var = currentEnviroment->get(exprVar->name);

            expr->index->accept(this);

            if (var.depth == 0)
                chunk->addCode(new InstSetGlobal(exprVar->name.getString(), expr->type, true));
            else
                chunk->addCode(new InstSetLocal(exprVar->name.getString(), currentEnviroment->get(exprVar->name), expr->type, true));
        }
        else if (expr->callee->instance == ExprType::Get)
        {
            ExprGet* get = (ExprGet*)expr->callee;

            if (get->callee->instance == ExprType::Variable)
            {
                expr->index->accept(this);
                TypeStruct* type = (TypeStruct*)get->callee->type.get();
                std::string getName = get->get.getString();
                ExprVariable* exprVar = (ExprVariable*)get->callee;
                Variable var = currentEnviroment->get(exprVar->name);
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
                chunk->addCode(new InstSetLocal(exprVar->name.getString(), var, expr->type, true));
            }
            else if (get->callee->instance == ExprType::GetDeref)
            {
                ExprGetDeref* exprGet = (ExprGetDeref*)get->callee;
                exprGet->callee->accept(this);
                expr->index->accept(this);
                TypeStruct* type = (TypeStruct*)exprGet->callee->type->intrinsicType.get();
                std::string getName = get->get.getString();
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
                chunk->addCode(new InstSetDerefOff(expr->type));
            }
        }

        // TODO: 2D Arrays
    }

    void visit(ExprAssignment* expr)
    {
        Variable var = currentEnviroment->get(expr->name);

        if (expr->assignment->instance == ExprType::Heap)
        {
            if (var.depth == 0)
                chunk->addCode(new InstGetGlobal(expr->name.getString(), expr->type));
            else
                chunk->addCode(new InstGetLocal(expr->name.getString(), var, expr->type));
            chunk->addCode(new InstFree());
        }

        expr->assignment->accept(this);
        if (var.depth == 0)
            chunk->addCode(new InstSetGlobal(expr->name.getString(), expr->type));
        else
            chunk->addCode(new InstSetLocal(expr->name.getString(), var, expr->type));
    }
    void visit(ExprBinary* expr)
    {
        expr->left->accept(this);
        expr->right->accept(this);

        switch (expr->op.type)
        {
        case TokenType::PLUS:
            chunk->addCode(new InstAdd(expr->left->type->tag));
            break;
        case TokenType::MINUS:
            chunk->addCode(new InstSub(expr->left->type->tag));
            break;
        case TokenType::STAR:
            chunk->addCode(new InstMul(expr->left->type->tag));
            break;
        case TokenType::SLASH:
            chunk->addCode(new InstDiv(expr->left->type->tag));
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
            chunk->addCode(new InstLess(expr->left->type->tag));
            break;
        case TokenType::LESS_EQUAL:
            chunk->addCode(new InstLte(expr->left->type->tag));
            break;
        case TokenType::GREAT:
            chunk->addCode(new InstGreat(expr->left->type->tag));
            break;
        case TokenType::GREAT_EQUAL:
            chunk->addCode(new InstGte(expr->left->type->tag));
            break;
        case TokenType::EQUAL_EQUAL:
            chunk->addCode(new InstEq(expr->left->type->tag));
            break;
        case TokenType::BANG_EQUAL:
            chunk->addCode(new InstNeq(expr->left->type->tag));
            break;
        }
    }
    void visit(ExprCall* expr)
    {
        std::vector<std::shared_ptr<Type>> args;
        for (auto& arg : expr->args)
        {
            arg->accept(this);
            args.push_back(arg->type);
        }

        expr->callee->accept(this);
        chunk->addCode(new InstCall(args, expr->callee->type->tag));
    }
    void visit(ExprCast* expr)
    {
        expr->expr->accept(this);
        chunk->addCode(new InstCast(expr->from->tag, expr->to->tag));
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
            chunk->addCode(new InstPop(makePrimTypeList({TypeTag::BOOL})));
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
            chunk->addCode(new InstPop(makePrimTypeList({TypeTag::BOOL})));
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
            chunk->addCode(new InstGetGlobal(expr->name.getString(), expr->type));
        else
            chunk->addCode(new InstGetLocal(expr->name.getString(), currentEnviroment->get(expr->name), expr->type));
    }
    void visit(ExprHeap* expr)
    {
        chunk->addCode(new InstAlloc(expr->type));
    }
    void visit(ExprGetDeref* expr)
    {
        expr->callee->accept(this);
        chunk->addCode(new InstGetDeref(expr->type));
    }
    void visit(ExprSetDeref* expr)
    {
        expr->asgn->accept(this);
        expr->callee->accept(this);
        chunk->addCode(new InstSetDeref(expr->type));
    }
    void visit(ExprRef* expr)
    {
        expr->callee->accept(this);
    }
    void visit(ExprTake* expr)
    {
        expr->source->accept(this);

        if (expr->source->instance == ExprType::Variable)
        {
            ExprVariable* e = (ExprVariable*)expr->source;
            Value* val = new Value();
            val->type = e->type;
            val->data.valPtr = nullptr;
            Expr* asgn = new ExprAssignment(e->name, new ExprLiteral(val));
            asgn->accept(this);
            chunk->addCode(new InstPop({e->type}));
        }
    }
    void visit(ExprGet* expr)
    {
        if (expr->callee->instance == ExprType::Variable)
        {
            TypeStruct* type = (TypeStruct*)expr->callee->type.get();
            std::string getName = expr->get.getString();
            ExprVariable* exprVar = (ExprVariable*)expr->callee;
            Variable var = currentEnviroment->get(exprVar->name);
            structMember m = type->members[getName];
            chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
            chunk->addCode(new InstGetLocal(exprVar->name.getString(), var, m.type, true));
        }
        else if (expr->callee->instance == ExprType::GetDeref)
        {
            ExprGetDeref* exprGet = (ExprGetDeref*)expr->callee;
            exprGet->callee->accept(this);
            TypeStruct* type = (TypeStruct*)exprGet->callee->type->intrinsicType.get();
            std::string getName = expr->get.getString();
            structMember m = type->members[getName];
            chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
            chunk->addCode(new InstGetDerefOff(m.type));
        }
    }

    void visit(ExprSet* expr)
    {
        expr->asgn->accept(this);
        if (expr->callee->instance == ExprType::Variable)
        {
            TypeStruct* type = (TypeStruct*)expr->callee->type.get();
            std::string getName = expr->get.getString();
            ExprVariable* exprVar = (ExprVariable*)expr->callee;
            Variable var = currentEnviroment->get(exprVar->name);
            structMember m = type->members[getName];
            chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
            chunk->addCode(new InstSetLocal(exprVar->name.getString(), var, m.type, true));
        }
        else if (expr->callee->instance == ExprType::GetDeref)
        {
            ExprGetDeref* exprGet = (ExprGetDeref*)expr->callee;
            exprGet->callee->accept(this);
            TypeStruct* type = (TypeStruct*)exprGet->callee->type->intrinsicType.get();
            std::string getName = expr->get.getString();
            structMember m = type->members[getName];
            chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
            chunk->addCode(new InstSetDerefOff(m.type));
        }
    }

    void visit(ExprAddr* expr)
    {
        if (expr->callee->instance == ExprType::Variable)
        {
            ExprVariable* exprVar = (ExprVariable*)(expr->callee);
            Variable var = currentEnviroment->get(exprVar->name);

            if (var.depth == 0)
                chunk->addCode(new InstAddrGlobal(exprVar->name.getString(), expr->type, false));
            else
                chunk->addCode(new InstAddrLocal(exprVar->name.getString(), currentEnviroment->get(exprVar->name), expr->type, false));
        }
        else if (expr->callee->instance == ExprType::ArrGet)
        {
            ExprArrGet* arrGet = (ExprArrGet*)expr->callee;
            ExprAddr* inner = new ExprAddr(arrGet->callee, expr->token);
            inner->accept(this);
            chunk->addCode(new InstConst(chunk->addConstant(new Value((int)(sizeof(Data) * arrGet->type->getSize())))));
            arrGet->index->accept(this);
            chunk->addCode(new InstMul(TypeTag::INTEGER));
            chunk->addCode(new InstAdd(TypeTag::INTEGER));
            // delete inner;
            // TODO: there is a leak.
        }
        else if (expr->callee->instance == ExprType::Get)
        {
            ExprGet* get = (ExprGet*)expr->callee;

            if (get->callee->instance == ExprType::Variable)
            {
                ExprAddr* inner = new ExprAddr(get->callee, expr->token);
                inner->accept(this);
                TypeStruct* type = (TypeStruct*)get->callee->type.get();
                std::string getName = get->get.getString();
                ExprVariable* exprVar = (ExprVariable*)get->callee;
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)(sizeof(Data) * (int)m.offset)))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
                delete inner;
            }
            else if (get->callee->instance == ExprType::GetDeref)
            {
                ExprGetDeref* exprGet = (ExprGetDeref*)get->callee;
                exprGet->callee->accept(this);
                TypeStruct* type = (TypeStruct*)exprGet->callee->type->intrinsicType.get();
                std::string getName = get->get.getString();
                structMember m = type->members[getName];
                chunk->addCode(new InstConst(chunk->addConstant(new Value((int)m.offset))));
                chunk->addCode(new InstAdd(TypeTag::INTEGER));
            }
        }
        else
        {
            std::cout << "[ERROR] Invalid address of at '" << expr->token.getString() << "', line " << expr->token.line << ".\n";
            cont = false;
        }
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
        chunk->addCode(new InstPop({stmt->expr->type}));
    }
    void visit(StmtFunc* stmt)
    {
        IRChunk* enclosing = chunk;
        chunk = new IRChunk(stmt->name.getString());

        FuncValue* func = (FuncValue*)globals[stmt->name.getString()];
        func->irChunk = chunk;
        func->data.valChunk = func->irChunk->chunk;

        beginScope(true);
        for (int i = 0; i < stmt->args.size(); i++)
            currentEnviroment->define(stmt->args[i], stmt->func_type->argTypes[i], true);

        for (auto& s : stmt->body->statements)
            s->accept(this);

        endScope();

        if (func->type->intrinsicType->tag == TypeTag::VOID)
            chunk->addCode(new InstReturn(std::make_shared<TypePrimitive>(TypeTag::VOID)));

        chunks.push_back(func->irChunk);

        chunk = enclosing;
    }
    void visit(StmtVarDecleration* stmt)
    {
        if (stmt->initializer == nullptr)
            chunk->addCode(new InstPush({stmt->varType}));
        else
            stmt->initializer->accept(this);

        if (currentEnviroment->depth != 0)
            currentEnviroment->define(stmt->name, stmt->varType, stmt->initializer);
        if (currentEnviroment->depth == 0)
        {
            chunk->addCode(new InstSetGlobal(stmt->name.getString(), stmt->varType));
            chunk->addCode(new InstPop({stmt->varType}));
        }
        else
            chunk->addCode(new InstSetLocal(stmt->name.getString(), currentEnviroment->get(stmt->name), stmt->varType));
    }
    void visit(StmtReturn* stmt)
    {
        auto vars = currentEnviroment->values;
        if (stmt->retVal && stmt->retVal->instance == ExprType::Variable)
        {
            for (auto& var : vars)
            {
                if (var.second.type->tag == TypeTag::POINTER && ((TypePointer*)(var.second.type.get()))->is_owner)
                {
                    if (((ExprVariable*)stmt->retVal)->name.getString() != var.first)
                    {
                        chunk->addCode(new InstGetLocal(var.first, var.second, var.second.type));
                        chunk->addCode(new InstFree());
                    }
                }
            }
        }
        else
        {
            for (auto& var : vars)
            {
                if (var.second.type->tag == TypeTag::POINTER && ((TypePointer*)(var.second.type.get()))->is_owner)
                {
                    chunk->addCode(new InstGetLocal(var.first, var.second, var.second.type));
                    chunk->addCode(new InstFree());
                }
            }
        }

        if (stmt->retVal != nullptr)
        {
            stmt->retVal->accept(this);
            chunk->addCode(new InstReturn(stmt->retVal->type));
        }
        else
        {
            chunk->addCode(new InstReturn(std::make_shared<TypePrimitive>(TypeTag::VOID)));
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
        {
            stmt->inc->accept(this);
            chunk->addCode(new InstPop({stmt->inc->type}));
        }
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
    void visit(StmtStruct* stmt)
    {
        for (auto& m : stmt->methodes)
        {
            m.second->accept(this);
        }
    }
    void visit(StmtCompUnit* stmt)
    {
        for(auto& s : stmt->stmts)
        {
            s->accept(this);
        }
    }
};