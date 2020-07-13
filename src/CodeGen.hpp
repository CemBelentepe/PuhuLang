#pragma once

#include "Chunk.hpp"
#include "IRChunk.hpp"
#include "InstVisitor.hpp"

struct constInfo
{
    size_t size;
    size_t addr;
    constInfo(size_t size, size_t addr)
        : size(size), addr(addr)
    {
    }
};

class CodeGen : public InstVisitor
{
private:
    IRChunk* irChunk;
    Chunk* chunk;
    std::vector<constInfo> constPositions;
    bool hadError;

public:
    CodeGen(IRChunk* irChunk)
        : irChunk(irChunk), chunk(new Chunk()), hadError(false)
    {
    }

    Chunk* generateCode()
    {
        for (auto& val : irChunk->getConstants())
        {
            size_t size = val->type.getSize();
            size_t addr = chunk->addConstant(val->cloneData(), size);
            constPositions.push_back(constInfo(size, addr));
        }

        for (auto& inst : irChunk->getCode())
        {
            inst->accept(this);
        }

        return chunk;
    }

    void error(const char* msg)
    {
        // TODO: add line to here
        this->hadError = true;
        std::cout << msg << std::endl;
    }

    void visit(InstConst* inst)
    {
        constInfo& c = constPositions[inst->id];
        if (c.addr > 255)
            error("Constant position is out of bounds[255].");

        chunk->addCode(OpCode::CONSTANT, c.size, c.addr);
    }
    void visit(InstCast* inst)
    {
        chunk->addCode(OpCode::CAST, (uint8_t)inst->from, (uint8_t)inst->to);
    }
    void visit(InstAdd* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = OpCode::IADD;
            break;
        case TypeTag::FLOAT:
            code = OpCode::FADD;
            break;
        case TypeTag::DOUBLE:
            code = OpCode::DADD;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstSub* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = OpCode::ISUB;
            break;
        case TypeTag::FLOAT:
            code = OpCode::FSUB;
            break;
        case TypeTag::DOUBLE:
            code = OpCode::DSUB;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstMul* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = OpCode::IMUL;
            break;
        case TypeTag::FLOAT:
            code = OpCode::FMUL;
            break;
        case TypeTag::DOUBLE:
            code = OpCode::DMUL;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstDiv* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = OpCode::IDIV;
            break;
        case TypeTag::FLOAT:
            code = OpCode::FDIV;
            break;
        case TypeTag::DOUBLE:
            code = OpCode::DDIV;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstNeg* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = OpCode::INEG;
            break;
        case TypeTag::FLOAT:
            code = OpCode::FNEG;
            break;
        case TypeTag::DOUBLE:
            code = OpCode::DNEG;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstMod* inst)
    {
        chunk->addCode(OpCode::MOD);
    }
    void visit(InstBit* inst)
    {
        OpCode code;
        switch (inst->op_type)
        {
        case TokenType::TILDE:
            code = OpCode::BIT_NOT;
            break;

        case TokenType::BIT_AND:
            code = OpCode::BIT_AND;
            break;

        case TokenType::BIT_OR:
            code = OpCode::BIT_OR;
            break;

        case TokenType::BIT_XOR:
            code = OpCode::BIT_XOR;
            break;

        case TokenType::BITSHIFT_LEFT:
            code = OpCode::BITSHIFT_LEFT;
            break;

        case TokenType::BITSHIFT_RIGHT:
            code = OpCode::BITSHIFT_RIGHT;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstNot* inst)
    {
        chunk->addCode(OpCode::LOGIC_NOT);
    }
    void visit(InstInc* inst)
    {
        OpCode code;
        switch (inst->type)
        {
        case TypeTag::INTEGER:
            code = inst->inc == 1 ? OpCode::IINC : OpCode::IDEC;
            break;
        case TypeTag::FLOAT:
            code = inst->inc == 1 ? OpCode::FINC : OpCode::FDEC;
            break;
        case TypeTag::DOUBLE:
            code = inst->inc == 1 ? OpCode::DINC : OpCode::DDEC;
            break;
        }
        chunk->addCode(code);
    }
    void visit(InstLess* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::ILESS);
        else
            chunk->addCode(OpCode::DLESS);
    }
    void visit(InstLte* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::ILESS_EQUAL);
        else
            chunk->addCode(OpCode::DLESS_EQUAL);
    }
    void visit(InstGreat* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::IGREAT);
        else
            chunk->addCode(OpCode::DGREAT);
    }
    void visit(InstGte* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::IGREAT_EQUAL);
        else
            chunk->addCode(OpCode::DGREAT_EQUAL);
    }
    void visit(InstEq* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::IIS_EQUAL);
        else
            chunk->addCode(OpCode::DIS_EQUAL);
    }
    void visit(InstNeq* inst)
    {
        if (inst->type == TypeTag::INTEGER)
            chunk->addCode(OpCode::INOT_EQUAL);
        else
            chunk->addCode(OpCode::DNOT_EQUAL);
    }
};
