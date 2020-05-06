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

	LOGIC_NOT, 
	DLESS, DGREAT, ILESS, IGREAT,
	DLESS_EQUAL, DGREAT_EQUAL, DIS_EQUAL, DNOT_EQUAL,
	ILESS_EQUAL, IGREAT_EQUAL, IIS_EQUAL, INOT_EQUAL,

	CAST,

	POPN,
	SET_GLOBAL, GET_GLOBAL, SET_LOCAL, GET_LOCAL,
	SET_GLOBAL_POP, SET_LOCAL_POP,
	GET_ARRAY, SET_ARRAY,

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
	std::vector<uint8_t*> constants;

	size_t addConstant(uint8_t* value, size_t valueSize);
	uint8_t* getConstant(size_t addr);
};
