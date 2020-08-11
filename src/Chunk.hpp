#pragma once

#include <cstdint>
#include <vector>
#include "Value.hpp"

enum class OpCode
{
	NOP,
	CONSTANT,

	IADD, ISUB, IMUL, IDIV, INEG, MOD,
	FADD, FSUB, FMUL, FDIV, FNEG,
	DADD, DSUB, DMUL, DDIV, DNEG,

	BIT_NOT, BIT_AND, BIT_OR,
	BIT_XOR, BITSHIFT_LEFT, BITSHIFT_RIGHT,

	IINC, IDEC,
	FINC, FDEC,
	DINC, DDEC,

	LOGIC_NOT, 
	DLESS, DGREAT, ILESS, IGREAT,
	DLESS_EQUAL, DGREAT_EQUAL, DIS_EQUAL, DNOT_EQUAL,
	ILESS_EQUAL, IGREAT_EQUAL, IIS_EQUAL, INOT_EQUAL,

	CAST,

	POPN, PUSHN,
	SET_GLOBAL, GET_GLOBAL, SET_LOCAL, GET_LOCAL,
	SET_GLOBALN, GET_GLOBALN, SET_LOCALN, GET_LOCALN,
	SET_GLOBAL_OFF, GET_GLOBAL_OFF, SET_LOCAL_OFF, GET_LOCAL_OFF,
	SET_GLOBAL_OFFN, GET_GLOBAL_OFFN, SET_LOCAL_OFFN, GET_LOCAL_OFFN,
	SET_GLOBAL_POP, SET_LOCAL_POP,

	ALLOC, FREE, SET_DEREF, GET_DEREF,

	JUMP, JUMP_NT_POP, LOOP,
	JUMP_NT,

	CALL, NATIVE_CALL,
	RETURN
};

class Chunk
{
public:
	std::vector<uint8_t> code;
	std::vector<Data> constants;
	
    Chunk() {}

	inline size_t addConstant(Data value)
    {
        size_t pos = constants.size();
        constants.push_back(value);
        return pos;
    }

    inline Data& getConstant(size_t addr)
    {
        return constants[addr];
    }

	inline void addCode(OpCode code)
	{
		this->code.push_back((uint8_t)code);
	}

	inline void addCode(OpCode code, uint8_t opr)
	{
		this->code.push_back((uint8_t)code);
		this->code.push_back(opr);
	}
	
	inline void addCode(OpCode code, uint8_t opr1, uint8_t opr2)
	{
		this->code.push_back((uint8_t)code);
		this->code.push_back(opr1);
		this->code.push_back(opr2);
	}
};