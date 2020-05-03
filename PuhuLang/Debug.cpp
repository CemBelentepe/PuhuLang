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
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << (unsigned int)chunk->code[++offset] << "\t" << (void*)chunk->getConstant(chunk->code[++offset]) << std::endl;
	return offset;
}

static size_t printPopInstruction(const char* name, Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << (unsigned int)chunk->code[++offset] << std::endl;
	return offset;
}

static size_t printLocalInstruction(const char* name, Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << (unsigned int)chunk->code[++offset] << " " << (unsigned int)chunk->code[++offset] << std::endl;
	return offset;
}

static size_t printJumpInstruction(const char* name, Chunk* chunk, size_t offset, int dir)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << dir * (int)(chunk->code[++offset]) + offset + 1 << std::endl;
	return offset;
}

static size_t printCastInstruction(Chunk* chunk, size_t offset)
{
	std::cout << offset << "\t" << std::setw(10) << std::left << "CAST" << "       from " << (int)(chunk->code[++offset]) << " to " << (int)(chunk->code[++offset]) << std::endl;
	return offset;
}

void printStack(VM& vm)
{
	std::cout << "\t";
	auto stack = vm.getStack();
	for(int i = 0; i < stack.count(); i++)
	{
		// std::cout << "[" << (void*)(val) << "]";
		printf("[%.2X]", stack[i]);
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
	case OpCode::DLESS:
		return printInstruction("DLESS", offset);
	case OpCode::DGREAT:
		return printInstruction("DGREAT", offset);
	case OpCode::DLESS_EQUAL:
		return printInstruction("DLESS_EQUAL", offset);
	case OpCode::DGREAT_EQUAL:
		return printInstruction("DGREAT_EQUAL", offset);
	case OpCode::DIS_EQUAL:
		return printInstruction("DIS_EQUAL", offset);
	case OpCode::DNOT_EQUAL:
		return printInstruction("DNOT_EQUAL", offset);
	case OpCode::ILESS:
		return printInstruction("ILESS", offset);
	case OpCode::IGREAT:
		return printInstruction("IGREAT", offset);
	case OpCode::ILESS_EQUAL:
		return printInstruction("ILESS_EQUAL", offset);
	case OpCode::IGREAT_EQUAL:
		return printInstruction("IGREAT_EQUAL", offset);
	case OpCode::IIS_EQUAL:
		return printInstruction("IIS_EQUAL", offset);
	case OpCode::INOT_EQUAL:
		return printInstruction("INOT_EQUAL", offset);
	case OpCode::CAST:
		return printCastInstruction(chunk, offset);
	case OpCode::POPN:
		return printPopInstruction("POPN", chunk, offset);
	case OpCode::SET_GLOBAL:
		return printLocalInstruction("SET_GLOBAL", chunk, offset);
	case OpCode::GET_GLOBAL:
		return printLocalInstruction("GET_GLOBAL", chunk, offset);
	case OpCode::SET_LOCAL:
		return printLocalInstruction("SET_LOCAL", chunk, offset);
	case OpCode::GET_LOCAL:
		return printLocalInstruction("GET_LOCAL", chunk, offset);
	case OpCode::SET_GLOBAL_POP:
		return printLocalInstruction("SET_GLOBAL_POP", chunk, offset);
	case OpCode::SET_LOCAL_POP:
		return printLocalInstruction("SET_LOCAL_POP", chunk, offset);
	case OpCode::JUMP:
		return printJumpInstruction("JUMP", chunk, offset, 1);
	case OpCode::JUMP_NT_POP:
		return printJumpInstruction("JUMP_NT_POP", chunk, offset, 1);
	case OpCode::LOOP:
		return printJumpInstruction("LOOP", chunk, offset, -1);
	case OpCode::JUMP_NT:
		return printJumpInstruction("JUMP_NT", chunk, offset, 1);
	case OpCode::CALL:
		return printPopInstruction("CALL", chunk, offset);
	case OpCode::NATIVE_CALL:
		return printPopInstruction("NATIVE", chunk, offset);
	case OpCode::RETURN:
		return printPopInstruction("RETURN", chunk, offset);
	default:
		return printInstruction("UNKNOWN", offset);
	}
}
