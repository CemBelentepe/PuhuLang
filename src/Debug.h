#pragma once

#include "AstVisitor.hpp"
#include "IRChunk.hpp"
#include "InstVisitor.hpp"
#include "Scanner.h"
#include "Value.h"
#include "ArrayList.h"
#include "Chunk.hpp"

#include <istream>
#include <iomanip>

void debugTokens(std::vector<Token> tokens);

class AstDebugger : public AstVisitor
{
public:
    void debugAll(Expr* expr)
    {
        expr->accept(this);
        std::cout << "\n";
    }

    void visit(ExprBinary* expr)
    {
        std::cout << "(" << expr->op.getString() << " ";
        expr->left->accept(this);
        std::cout << ", ";
        expr->right->accept(this);
        std::cout << ")";
    }

    void visit(ExprCall* expr)
    {
        std::cout << "(";
        expr->callee->accept(this);
        for (int i = 0; i < expr->args.size(); i++)
        {
            if (i > 0)
                std::cout << ", ";
            expr->args[i]->accept(this);
        }
        std::cout << ")";
    }

    void visit(ExprCast* expr)
    {
        std::cout << "(";
        expr->expr->accept(this);
        std::cout << " " << expr->from << "->" << expr->to << ")";
    }

    void visit(ExprUnary* expr)
    {
        std::cout << "(" << expr->op.getString();
        expr->expr->accept(this);
        std::cout << ")";
    }

    void visit(ExprVariable* expr)
    {
        std::cout << expr->name;
    }

    void visit(ExprLiteral* expr)
    {
        std::cout << *(expr->val);
    }
};

void debugAST(Expr* expr);

class InstDebugger : public InstVisitor
{
private:
    IRChunk* irChunk;

public:
    InstDebugger(IRChunk* irChunk)
        : irChunk(irChunk)
    {
    }

    void debugAll()
    {
        for (auto& inst : irChunk->getCode())
        {
            inst->accept(this);
            std::cout << "\n";
        }
    }

    void visit(InstConst* inst)
    {
        std::cout << "CONST\t" << *(irChunk->getConstant(inst->id)) << "@[" << inst->id << "]";
    }
    void visit(InstCast* inst)
    {
        std::cout << "CAST\t" << Type(inst->from) << "\t" << Type(inst->to);
    }
    void visit(InstAdd* inst)
    {
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "IADD";
            break;
        case TypeTag::FLOAT:
            std::cout << "FADD";
            break;
        case TypeTag::DOUBLE:
            std::cout << "DADD";
            break;
        }
    }
    void visit(InstSub* inst)
    {
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "ISUB";
            break;
        case TypeTag::FLOAT:
            std::cout << "FSUB";
            break;
        case TypeTag::DOUBLE:
            std::cout << "DSUB";
            break;
        }
    }
    void visit(InstMul* inst)
    {
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "IMUL";
            break;
        case TypeTag::FLOAT:
            std::cout << "FMUL";
            break;
        case TypeTag::DOUBLE:
            std::cout << "DMUL";
            break;
        }
    }
    void visit(InstDiv* inst)
    {
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "IDIV";
            break;
        case TypeTag::FLOAT:
            std::cout << "FDIV";
            break;
        case TypeTag::DOUBLE:
            std::cout << "DDIV";
            break;
        }
    }
    void visit(InstNeg* inst)
    {
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "INEG";
            break;
        case TypeTag::FLOAT:
            std::cout << "FNEG";
            break;
        case TypeTag::DOUBLE:
            std::cout << "DNEG";
            break;
        }
    }
    void visit(InstMod* inst)
    {
        std::cout << "MOD";
    }
    void visit(InstBit* inst)
    {
        switch (inst->op_type)
        {
        case TokenType::TILDE:
            std::cout << "BIT_NOT";
            break;

        case TokenType::BIT_AND:
            std::cout << "BIT_AND";
            break;

        case TokenType::BIT_OR:
            std::cout << "BIT_OR";
            break;

        case TokenType::BIT_XOR:
            std::cout << "BIT_XOR";
            break;

        case TokenType::BITSHIFT_LEFT:
            std::cout << "BITSHIFT_LEFT";
            break;

        case TokenType::BITSHIFT_RIGHT:
            std::cout << "BITSHIFT_RIGHT";
            break;
        }
    }
    void visit(InstNot* inst)
    {
        std::cout << "LOGIC_NOT";
    }
    void visit(InstInc* inst)
    {
        std::string inc = inst->inc == 1 ? "INC" : "DEC";
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            std::cout << "I";
            break;
        case TypeTag::FLOAT:
            std::cout << "F";
            break;
        case TypeTag::DOUBLE:
            std::cout << "D";
            break;
        }
        std::cout << inc;
    }
    void visit(InstLess* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "ILESS";
        else
            std::cout << "DLESS";
    }
    void visit(InstLte* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "ILESS_EQUAL";
        else
            std::cout << "DLESS_EQUAL";
    }
    void visit(InstGreat* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "IGREAT";
        else
            std::cout << "DGREAT";
    }
    void visit(InstGte* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "IGREAT_EQUAL";
        else
            std::cout << "DGREAT_EQUAL";
    }
    void visit(InstEq* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "IIS_EQUAL";
        else
            std::cout << "DIS_EQUAL";
    }
    void visit(InstNeq* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            std::cout << "INOT_EQUAL";
        else
            std::cout << "DNOT_EQUAL";
    }
};

void debugInstructions(IRChunk* irChunk);

void printStack(ArrayList<uint8_t>& stack);

size_t dissambleInstruction(Chunk* chunk, size_t offset);

void dissambleChunk(Chunk* chunk);
