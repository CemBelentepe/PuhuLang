#include "Debug.h"
#include <iostream>
#include <iomanip>

static size_t printInstruction(const char* name, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << std::endl;
	return offset;
}

static size_t printConstantInstruction(const char* name, Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << *(chunk->getConstant(chunk->code[++offset])) << std::endl;
	return offset;
}

static size_t printLocalInstruction(const char* name, Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << (unsigned int)chunk->code[++offset] << std::endl;
	return offset;
}

static size_t printJumpInstruction(const char* name, Chunk* chunk, size_t offset, int dir)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << dir * (int)(chunk->code[++offset]) + offset + 1 << std::endl;
	return offset;
}

static size_t printCastInstruction(Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << "CAST" << "       to " << (int)(chunk->code[++offset]) << std::endl;
	return offset;
}

void printStack(VM& vm)
{
	std::cout << "\t";
	for (const Value* val : vm.getStack())
	{
		std::cout << "[" << (*val) << "]";
	}
	std::cout << std::endl;
}

void dissableChunk(Chunk* chunk)
{
	size_t offset = 0;
	while (offset < chunk->code.size())
	{
		offset = dissambleInstruction(chunk, offset) + 1;
	}
}

size_t dissambleInstruction(Chunk* chunk, size_t offset)
{
	OpCode code = (OpCode)(chunk->code[offset]);
	std::cout << "\t";
	switch (code)
	{
	case OpCode::CONSTANT:
		return printConstantInstruction("CONSTANT", chunk, offset);
	case OpCode::IADD:
		return printInstruction("IADD", offset);
	case OpCode::ISUB:
		return printInstruction("ISUB", offset);
	case OpCode::IMUL:
		return printInstruction("IMUL", offset);
	case OpCode::IDIV:
		return printInstruction("IDIV", offset);
	case OpCode::INEG:
		return printInstruction("INEG", offset);
	case OpCode::MOD:
		return printInstruction("MOD", offset);
	case OpCode::FADD:
		return printInstruction("FADD", offset);
	case OpCode::FSUB:
		return printInstruction("FSUB", offset);
	case OpCode::FMUL:
		return printInstruction("FMUL", offset);
	case OpCode::FDIV:
		return printInstruction("FDIV", offset);
	case OpCode::FNEG:
		return printInstruction("FNEG", offset);
	case OpCode::DADD:
		return printInstruction("DADD", offset);
	case OpCode::DSUB:
		return printInstruction("DSUB", offset);
	case OpCode::DMUL:
		return printInstruction("DMUL", offset);
	case OpCode::DDIV:
		return printInstruction("DDIV", offset);
	case OpCode::DNEG:
		return printInstruction("DNEG", offset);
	case OpCode::BIT_NOT:
		return printInstruction("BIT_NOT", offset);
	case OpCode::BIT_AND:
		return printInstruction("BIT_AND", offset);
	case OpCode::BIT_OR:
		return printInstruction("BIT_OR", offset);
	case OpCode::BIT_XOR:
		return printInstruction("BIT_XOR", offset);
	case OpCode::BITSHIFT_LEFT:
		return printInstruction("BIT_LEFT", offset);
	case OpCode::BITSHIFT_RIGHT:
		return printInstruction("BIT_RIGHT", offset);
	case OpCode::IPRE_INC:
		return printInstruction("IPRE_INC", offset);
	case OpCode::IPRE_DEC:
		return printInstruction("IPRE_DEC", offset);
	case OpCode::IPOST_INC:
		return printInstruction("IPOST_INC", offset);
	case OpCode::IPOST_DEC:
		return printInstruction("IPOST_DEC", offset);
	case OpCode::FPRE_INC:
		return printInstruction("FPRE_INC", offset);
	case OpCode::FPRE_DEC:
		return printInstruction("FPRE_DEC", offset);
	case OpCode::FPOST_INC:
		return printInstruction("FPOST_INC", offset);
	case OpCode::FPOST_DEC:
		return printInstruction("FPOST_DEC", offset);
	case OpCode::DPRE_INC:
		return printInstruction("DPRE_INC", offset);
	case OpCode::DPRE_DEC:
		return printInstruction("DPRE_DEC", offset);
	case OpCode::DPOST_INC:
		return printInstruction("DPOST_INC", offset);
	case OpCode::DPOST_DEC:
		return printInstruction("DPOST_DEC", offset);
	case OpCode::LOGIC_NOT:
		return printInstruction("LOGIC_NOT", offset);
	case OpCode::LOGIC_AND:
		return printInstruction("LOGIC_AND", offset);
	case OpCode::LOGIC_OR:
		return printInstruction("LOGIC_OR", offset);
	case OpCode::LESS:
		return printInstruction("LESS", offset);
	case OpCode::GREAT:
		return printInstruction("GREAT", offset);
	case OpCode::LESS_EQUAL:
		return printInstruction("LESS_EQUAL", offset);
	case OpCode::GREAT_EQUAL:
		return printInstruction("GREAT_EQUAL", offset);
	case OpCode::IS_EQUAL:
		return printInstruction("IS_EQUAL", offset);
	case OpCode::NOT_EQUAL:
		return printInstruction("NOT_EQUAL", offset);
	case OpCode::CAST:
		return printCastInstruction(chunk, offset);
	case OpCode::POP:
		return printInstruction("POP", offset); 
	case OpCode::POPN:
		return printLocalInstruction("POPN", chunk, offset);
	case OpCode::SET_GLOBAL:
		return printConstantInstruction("SET_GLOBAL", chunk, offset);
	case OpCode::GET_GLOBAL:
		return printConstantInstruction("GET_GLOBAL", chunk, offset);
	case OpCode::SET_LOCAL:
		return printLocalInstruction("SET_LOCAL", chunk, offset);
	case OpCode::GET_LOCAL:
		return printLocalInstruction("GET_LOCAL", chunk, offset);
	case OpCode::JUMP:
		return printJumpInstruction("JUMP", chunk, offset, 1);
	case OpCode::JUMP_NT:
		return printJumpInstruction("JUMP_NT", chunk, offset, 1);
	case OpCode::LOOP:
		return printJumpInstruction("LOOP", chunk, offset, -1);
	case OpCode::CALL:
		return printLocalInstruction("CALL", chunk, offset);
	case OpCode::NATIVE_CALL:
		return printLocalInstruction("NATIVE", chunk, offset);
	case OpCode::RETURN:
		return printInstruction("RETURN", offset);
	default:
		return printInstruction("UNKNOWN", offset);
	}
}
