#pragma once
#include <vector>
#include "Value.h"

enum class OpCode
{
	CONSTANT,

	IADD, ISUB, IMUL, IDIV, INEG, MOD,
	FADD, FSUB, FMUL, FDIV, FNEG,
	DADD, DSUB, DMUL, DDIV, DNEG,

	BIT_NOT, BIT_AND, BIT_OR,
	BIT_XOR, BITSHIFT_LEFT, BITSHIFT_RIGHT,

	IPRE_INC, IPRE_DEC, IPOST_INC, IPOST_DEC,
	FPRE_INC, FPRE_DEC, FPOST_INC, FPOST_DEC,
	DPRE_INC, DPRE_DEC, DPOST_INC, DPOST_DEC,

	LOGIC_NOT, LOGIC_AND, LOGIC_OR, LESS, GREAT, 
	LESS_EQUAL, GREAT_EQUAL, IS_EQUAL, NOT_EQUAL,

	CAST,

	POP, POPN,
	SET_GLOBAL, GET_GLOBAL, SET_LOCAL, GET_LOCAL,

	JUMP, JUMP_NT_POP, LOOP,
	JUMP_NT,

	CALL, NATIVE_CALL,
	RETURN
};

class Chunk
{
public:
	Chunk();
	~Chunk();
	std::vector<uint8_t> code;
	std::vector<Value*> constants;

	size_t addConstant(Value* value);
	Value* getConstant(size_t addr);
};

