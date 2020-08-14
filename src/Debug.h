#pragma once

#include "AstVisitor.hpp"
#include "Chunk.hpp"
#include "IRChunk.hpp"
#include "InstVisitor.hpp"
#include "Scanner.h"
#include "Value.hpp"

#include <iomanip>
#include <istream>

void debugTokens(std::vector<Token> tokens);

class AstDebugger : public AstVisitor
{
private:
    size_t indent;
    void indentCode()
    {
        for (size_t i = 0; i < indent; i++)
            std::cout << "    ";
        std::cout << "|";
    }

public:
    void debugAll(std::vector<Stmt*>& root)
    {
        indent = 0;
        for (auto& stmt : root)
            stmt->accept(this);
        std::cout << "\n";
    }

    void visit(ExprArrGet* expr)
    {
        std::cout << "(";
        expr->callee->accept(this);
        std::cout << "[";
        expr->index->accept(this);
        std::cout << "])";
    }

    void visit(ExprArrSet* expr)
    {
        std::cout << "(";
        expr->callee->accept(this);
        std::cout << "[";
        expr->index->accept(this);
        std::cout << "] = ";
        expr->assignment->accept(this);
        std::cout << ")";
    }

    void visit(ExprAssignment* expr)
    {
        std::cout << "(" << expr->name.getString() << " = ";
        expr->assignment->accept(this);
        std::cout << ")";
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
        std::cout << expr->name.getString();
    }

    void visit(ExprLiteral* expr)
    {
        std::cout << *(expr->val);
    }

    void visit(ExprLogic* expr)
    {
        std::cout << "(" << expr->op.getString() << " ";
        expr->left->accept(this);
        std::cout << ", ";
        expr->right->accept(this);
        std::cout << ")";
    }

    void visit(ExprHeap* expr)
    {
        std::cout << "(heap " << expr->constructType->getName().str() << ")";
    }

    void visit(ExprGetDeref* expr)
    {
        std::cout << "(^ ";
        expr->callee->accept(this);
        std::cout << ")";
    }

    void visit(ExprSetDeref* expr)
    {
        std::cout << "(^ ";
        expr->callee->accept(this);
        std::cout << " = ";
        expr->asgn->accept(this);
        std::cout << ")";
    }

    void visit(ExprRef* expr)
    {
        std::cout << "(ref ";
        expr->callee->accept(this);
        std::cout << ")";
    }

    void visit(ExprTake* expr)
    {
        std::cout << "(take ";
        expr->source->accept(this);
        std::cout << ")";
    }

    void visit(ExprGet* expr)
    {
        std::cout << "(";
        expr->callee->accept(this);
        std::cout << "." << expr->get.getString() << ")";
    }

    void visit(ExprSet* expr)
    {
        std::cout << "(";
        expr->callee->accept(this);
        std::cout << "." << expr->get.getString() << " = ";
        expr->asgn->accept(this);
        std::cout << ")";
    }

    void visit(StmtBlock* stmt)
    {
        indentCode();

        std::cout << "-> Block:\n";
        indent++;
        for (auto& s : stmt->statements)
        {
            s->accept(this);
        }
        indent--;
        std::cout << "\n";
    }
    void visit(StmtExpr* stmt)
    {
        indentCode();
        std::cout << "-> Expression: ";
        stmt->expr->accept(this);
        std::cout << "\n";
    }
    void visit(StmtFunc* stmt)
    {
        indentCode();
        std::cout << "-> " << stmt->name.getString() << "(";
        indent++;

        for (size_t i = 0; i < stmt->args.size(); i++)
        {
            if (i != 0)
                std::cout << ", ";
            stmt->func_type->argTypes[i]->print();
            std::cout << " " << stmt->args[i].getString();
        }
        std::cout << ")\n";

        stmt->body->accept(this);
        indent--;
    }
    void visit(StmtVarDecleration* stmt)
    {
        indentCode();
        std::cout << "-> " << stmt->varType->getName().str() << " " << stmt->name.getString();
        if (stmt->initializer != nullptr)
        {
            std::cout << " = ";
            stmt->initializer->accept(this);
        }
        std::cout << "\n";
    }
    void visit(StmtReturn* stmt)
    {
        indentCode();
        std::cout << "-> return ";
        if (stmt->retVal != nullptr)
        {
            stmt->retVal->accept(this);
        }
        std::cout << "\n";
    }
    void visit(StmtIf* stmt)
    {
        indentCode();
        std::cout << "-> if:\n";
        indentCode();
        std::cout << "Condition: ";
        stmt->condition->accept(this);
        std::cout << "\n";
        indentCode();
        indent++;
        std::cout << "Then:\n";
        stmt->then->accept(this);
        if (stmt->els != nullptr)
        {
            std::cout << "Else:\n";
            stmt->then->accept(this);
        }
        indent--;
        std::cout << "\n";
    }
    void visit(StmtFor* stmt)
    {
        indentCode();
        std::cout << "-> for:\n";
        indent++;
        if (stmt->decl)
        {
            indentCode();
            std::cout << "Decleration:\n";
            stmt->decl->accept(this);
            std::cout << "\n";
        }
        indentCode();
        std::cout << "Condition: ";
        stmt->cond->accept(this);
        std::cout << "\n";
        if (stmt->inc)
        {
            indentCode();
            std::cout << "Incrementation: ";
            stmt->inc->accept(this);
            std::cout << "\n";
        }
        indentCode();
        stmt->loop->accept(this);
        indent--;
        std::cout << "\n";
    }
    void visit(StmtWhile* stmt)
    {
        indentCode();
        std::cout << "-> while:\n";
        indent++;
        indentCode();
        std::cout << "Condition: ";
        stmt->condition->accept(this);
        std::cout << "\n";
        indentCode();
        stmt->loop->accept(this);
        indent--;
        std::cout << "\n";
    }
    void visit(StmtClass* stmt)
    {
        indentCode();
        std::cout << "->class " << stmt->type->name.getString() << "\n";
        indent++;
        for(auto& var : stmt->type->members)
        {
            indentCode();
            std::cout << var.second.type->getName().str() << " " << var.second.name.getString() << "\n";
        }

        std::cout << "\n";
        indent--;
    }
};

void debugAST(std::vector<Stmt*>& expr);

class InstDebugger : public InstVisitor
{
private:
    IRChunk* irChunk;

public:
    InstDebugger(IRChunk* irChunk)
        : irChunk(irChunk)
    {
    }

    void printTag(TypeTag tag)
    {
        TypePrimitive t(tag);
        t.print();
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
        std::cout << "CAST\t";
        printTag(inst->from);
        std::cout << "\t";
        printTag(inst->to);
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
    void visit(InstGetGlobal* inst)
    {
        std::cout << "GET_GLOBAL\t" << inst->name;
    }
    void visit(InstSetGlobal* inst)
    {
        std::cout << "SET_GLOBAL\t" << inst->name;
    }
    void visit(InstGetLocal* inst)
    {
        std::cout << "GET_LOCAL\t" << inst->name;
    }
    void visit(InstSetLocal* inst)
    {
        std::cout << "SET_LOCAL\t" << inst->name;
    }
    void visit(InstAlloc* inst)
    {
        std::cout << "ALLOC\t" << inst->type;
    }
    void visit(InstFree* inst)
    {
        std::cout << "FREE";
    }
    void visit(InstGetDeref* inst)
    {
        std::cout << "GET_DEREF\t" << inst->type;
    }
    void visit(InstSetDeref* inst)
    {
        std::cout << "SET_DEREF\t" << inst->type;
    }
    void visit(InstGetDerefOff* inst)
    {
        std::cout << "GET_DEREF_OFF\t" << inst->type;
    }
    void visit(InstSetDerefOff* inst)
    {
        std::cout << "SET_DEREF_OFF\t" << inst->type;
    }
    void visit(InstCall* inst)
    {
        if (inst->callType == TypeTag::NATIVE)
            std::cout << "NATIVE_";
        std::cout << "CALL\t";
        for (auto& arg : inst->args)
        {
            arg->print();
            std::cout << " ";
        }
    }
    void visit(InstPop* inst)
    {
        std::cout << "POP\t";
        for (auto& arg : inst->types)
        {
            arg->print();
            std::cout << " ";
        }
    }
    void visit(InstPush* inst)
    {
        std::cout << "PUSH\t";
        for (auto& arg : inst->types)
        {
            arg->print();
            std::cout << " ";
        }
    }
    void visit(InstReturn* inst)
    {
        std::cout << "RETURN\t";
        inst->type->print();
    }
    void visit(InstJump* inst)
    {
        switch (inst->type)
        {
        case 0:
            std::cout << "JUMP";
            break;
        case 1:
            std::cout << "JUMP_NT";
            break;
        case 2:
            std::cout << "JUMP_NT_POP";
            break;
        }
        std::cout << "\tL." << inst->label->id;
    }
    void visit(InstLabel* inst)
    {
        std::cout << "L." << inst->id << ":";
    }
};

void debugInstructions(IRChunk* irChunk);

void printStack(std::vector<Data>& stack);

size_t dissambleInstruction(Chunk* chunk, size_t offset);

void dissambleChunk(Chunk* chunk);
