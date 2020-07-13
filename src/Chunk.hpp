#pragma once

#include "ArrayList.h"
#include <cstdint>
#include <vector>

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
	std::vector<uint8_t> code;
	ArrayList<uint8_t> constants;
	
    Chunk() {}

	inline size_t addConstant(uint8_t* value, size_t valueSize)
    {
        size_t pos = constants.count();
        constants.push_sized(value, valueSize);
        return pos;
    }

    template<class T>
    inline size_t addConstant(T value)
    {
        size_t pos = constants.count();
        constants.push_as<T>(value);
        return pos;
    }

    inline uint8_t* getConstant(size_t addr)
    {
        return constants.get_at_ref(addr);
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