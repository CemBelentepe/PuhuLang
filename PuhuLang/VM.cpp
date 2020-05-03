#include "VM.h"

#ifdef _DEBUG
#include "Debug.h"
#endif // DEBUG_CODE_TRACE


VM::VM()
	:ip(0), currentChunk(nullptr)
{}

VM::~VM()
{}

bool VM::interpret(Chunk* entryChunk)
{
	this->currentChunk = entryChunk;
	while (ip < this->currentChunk->code.size())
	{
		run((OpCode)advance());
	}
	return true;
}

void VM::run(OpCode code)
{
#ifdef _DEBUG
	printStack(*this);
	dissambleInstruction(currentChunk, this->ip - 1);
#endif // DEBUG_CODE_TRACE

#define BINARY_OP(op, type)\
	    do{\
		type b = pop<type>();\
		type* a = (type*)(&stack[stack.size() - sizeof(type)]);\
		*a = *a op b; \
	    }while(false)\

#define DCMP_OP(op)\
	    do{\
		double b = pop<double>();\
		double a = pop<double>();\
		push<bool>(a op b);\
	    }while(false)\

#define ICMP_OP(op)\
	    do{\
		int32_t b = pop<int32_t>();\
		int32_t a = pop<int32_t>();\
		push<bool>(a op b);\
	    }while(false)\

	switch (code)
	{
	case OpCode::CONSTANT:
	{
		uint8_t size = advance();
		uint8_t* constant = currentChunk->getConstant(advance());
		pushSized(constant, size);
		break;
	}
	case OpCode::IADD:
	{
		BINARY_OP(+, int32_t);
		break;
	}
	case OpCode::ISUB:
	{
		BINARY_OP(-, int32_t);
		break;
	}
	case OpCode::IMUL:
	{
		BINARY_OP(*, int32_t);
		break;
	}
	case OpCode::IDIV:
	{
		BINARY_OP(/ , int32_t);
		break;
	}
	case OpCode::INEG:
	{
		push<int32_t>(-1 * pop<int32_t>());
		break;
	}
	case OpCode::MOD:
	{
		BINARY_OP(%, int32_t);
		break;
	}
	case OpCode::FADD:
	{
		BINARY_OP(+, float);
		break;
	}
	case OpCode::FSUB:
	{
		BINARY_OP(-, float);
		break;
	}
	case OpCode::FMUL:
	{
		BINARY_OP(*, float);
		break;
	}
	case OpCode::FDIV:
	{
		BINARY_OP(/ , float);
		break;
	}
	case OpCode::FNEG:
	{
		push<float>(-1 * pop<float>());
		break;
	}
	case OpCode::DADD:
	{
		BINARY_OP(+, double);
		break;
	}
	case OpCode::DSUB:
	{
		BINARY_OP(-, double);
		break;
	}
	case OpCode::DMUL:
	{
		BINARY_OP(*, double);
		break;
	}
	case OpCode::DDIV:
	{
		BINARY_OP(/ , double);
		break;
	}
	case OpCode::DNEG:
	{
		push<double>(-1 * pop<double>());
		break;
	}
	case OpCode::BIT_NOT:
	{
		push<int32_t>(~pop<int32_t>());
		break;
	}
	case OpCode::BIT_AND:
	{
		BINARY_OP(&, int32_t);
		break;
	}
	case OpCode::BIT_OR:
	{
		BINARY_OP(| , int32_t);
		break;
	}
	case OpCode::BIT_XOR:
	{
		BINARY_OP(^, int32_t);
		break;
	}
	case OpCode::BITSHIFT_LEFT:
	{
		BINARY_OP(<< , int32_t);
		break;
	}
	case OpCode::BITSHIFT_RIGHT:
	{
		BINARY_OP(>> , int32_t);
		break;
	}
	case OpCode::IPRE_INC:
	{
		push<int32_t>(pop<int32_t>() + 1);
		break;
	}
	case OpCode::IPRE_DEC:
	{
		push<int32_t>(pop<int32_t>() - 1);
		break;
	}
	case OpCode::IPOST_INC:
		break;
	case OpCode::IPOST_DEC:
		break;
	case OpCode::FPRE_INC:
	{
		push<float>(pop<float>() + 1);
		break;
	}
	case OpCode::FPRE_DEC:
	{
		push<float>(pop<float>() - 1);
		break;
	}
	case OpCode::FPOST_INC:
		break;
	case OpCode::FPOST_DEC:
		break;
	case OpCode::DPRE_INC:
	{
		push<double>(pop<double>() + 1);
		break;
	}
	case OpCode::DPRE_DEC:
	{
		push<double>(pop<double>() - 1);
		break;
	}
	case OpCode::DPOST_INC:
		break;
	case OpCode::DPOST_DEC:
		break;
	case OpCode::LOGIC_NOT:
	{
		push<bool>(!pop<bool>());
		break;
	}
	case OpCode::DLESS:
	{
		DCMP_OP(< );
		break;
	}
	case OpCode::DGREAT:
	{
		DCMP_OP(> );
		break;
	}
	case OpCode::ILESS:
	{
		ICMP_OP(< );
		break;
	}
	case OpCode::IGREAT:
	{
		ICMP_OP(> );
		break;
	}
	case OpCode::DLESS_EQUAL:
	{
		DCMP_OP(<= );
		break;
	}
	case OpCode::DGREAT_EQUAL:
	{
		DCMP_OP(>= );
		break;
	}
	case OpCode::DIS_EQUAL:
	{
		DCMP_OP(== );
		break;
	}
	case OpCode::DNOT_EQUAL:
	{
		ICMP_OP(!= );
		break;
	}
	case OpCode::ILESS_EQUAL:
	{
		ICMP_OP(<= );
		break;
	}
	case OpCode::IGREAT_EQUAL:
	{
		ICMP_OP(>= );
		break;
	}
	case OpCode::IIS_EQUAL:
	{
		ICMP_OP(== );
		break;
	}
	case OpCode::INOT_EQUAL:
	{
		ICMP_OP(!= );
		break;
	}
	case OpCode::CAST:
	{
		ValueType from = (ValueType)advance();
		ValueType to = (ValueType)advance();
		typeCast(from, to);
		break;
	}
	case OpCode::POPN:
	{
		int toPop = advance();
		for (int i = 0; i < toPop; i++)
		{
			this->stack.pop_back();
		}
		break;
	}
	case OpCode::SET_GLOBAL:
	{
		size_t size = advance();
		size_t slot = advance();
		uint8_t* val = peekSized(size);
		for (int i = 0; i < size; i++)
		{
			(*(uint8_t**)this->globals[slot])[i] = val[i];
		}
		break;
	}
	case OpCode::GET_GLOBAL:
	{
		size_t size = advance();
		size_t slot = advance();
		uint8_t* val = *(uint8_t**)(this->globals[slot]);
		pushSized(val, size);
		break;
	}
	case OpCode::SET_LOCAL:
	{
		uint8_t slot = advance();
		uint8_t size = advance();
		for (int i = 0; i < size; i++)
		{
			stack[slot + this->frames.back().frameStart + i] = stack[stack.size() - size + i];
		}

		break;
	}
	case OpCode::GET_LOCAL:
	{
		uint8_t slot = advance();
		uint8_t size = advance();

		pushSized(&stack[slot + this->frames.back().frameStart], size);
		break;
	}
	case OpCode::SET_GLOBAL_POP:
	{
		size_t size = advance();
		size_t slot = advance();
		uint8_t* val = peekSized(size);
		for (int i = 0; i < size; i++)
		{
			(*(uint8_t**)this->globals[slot])[i] = val[i];
		}
		popSized(size);
		break;
	}
	case OpCode::SET_LOCAL_POP:
	{
		uint8_t slot = advance();
		uint8_t size = advance();
		for (int i = 0; i < size; i++)
		{
			stack[slot + this->frames.back().frameStart + i] = stack[stack.size() - size + i];
		}
		popSized(size);
		break;
	}
	case OpCode::JUMP:
	{
		uint8_t offset = advance();
		this->ip += offset;
		break;
	}
	case OpCode::JUMP_NT_POP:
	{
		uint8_t offset = advance();
		if (!pop<bool>())
			this->ip += offset;
		break;
	}
	case OpCode::LOOP:
	{
		uint8_t offset = advance();
		this->ip -= offset;
		break;
	}
	case OpCode::JUMP_NT:
	{
		uint8_t offset = advance();
		if (!peek<bool>())
			this->ip += offset;
		break;
	}
	case OpCode::CALL:
	{
		uint8_t argSize = advance();
		Chunk* func = *(Chunk**)(&this->stack[this->stack.size() - sizeof(Chunk**) - argSize]);
		this->frames.push_back(Frame(ip, currentChunk, this->stack.size() - argSize - sizeof(Chunk**)));
		this->currentChunk = func;
		this->ip = 0;
		break;
	}
	case OpCode::NATIVE_CALL:
	{
		uint8_t argSize = advance();
		NativeFunc* call = *(NativeFunc**)(&stack[stack.size() - sizeof(NativeFunc**) - argSize]);
		NativeFn func = call->func;
		uint8_t* result = func(argSize, argSize > 0 ? &(this->stack[this->stack.size() - argSize]) : nullptr);

		this->stack.resize(this->stack.size() - argSize - sizeof(NativeFunc**));

		pushSized(result, call->type.intrinsicType->getSize());
		break;
	}
	case OpCode::RETURN:
	{
		uint8_t size = advance();
		uint8_t* retVal = nullptr;

		if (size != 0)
			retVal = popSized(size);

		auto frame = this->frames.back();
		this->frames.pop_back();

		this->stack.resize(frame.frameStart);

		this->ip = frame.ip;
		this->currentChunk = frame.chunk;
		pushSized(retVal, size);
		break;
	}
	}
}

uint8_t VM::advance()
{
	return this->currentChunk->code[this->ip++];
}

void VM::typeCast(ValueType from, ValueType to)
{
#define CAST(value)   \
	switch (to)\
	{\
	case ValueType::INTEGER:\
		push<int32_t>(value);\
		break;\
	case ValueType::FLOAT:\
		push<float>(value);\
		break;\
	case ValueType::DOUBLE:\
		push<double>(value);\
		break;\
	case ValueType::BOOL:\
		push<bool>(value);\
		break;\
	case ValueType::CHAR:\
		push<char>(value);\
		break;\
	}\

	switch (from)
	{
	case ValueType::INTEGER:
	{
		CAST(pop<int32_t>());
		break;
	}
	case ValueType::FLOAT:
	{
		CAST(pop<float>());
		break;
	}
	case ValueType::DOUBLE:
	{
		CAST(pop<double>());
		break;
	}
	case ValueType::BOOL:
	{
		CAST(pop<bool>());
		break;
	}
	case ValueType::CHAR:
	{
		CAST(pop<char>());
		break;
	}
	}
}
