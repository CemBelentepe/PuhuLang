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
#ifdef _DEBUG
		printStack(*this);
		dissambleInstruction(currentChunk, this->ip - 1);
#endif // DEBUG_CODE_TRACE

#define BINARY_OP(op, type)\
	    do{\
		type b = stack.pop_as<type>();\
		type* a = stack.peek_as_ref<type>();\
		*a = *a op b; \
	    }while(false)\

#define DCMP_OP(op)\
	    do{\
		double b = stack.pop_as<double>();\
		double a = stack.pop_as<double>();\
		stack.push_as<bool>(a op b);\
	    }while(false)\

#define ICMP_OP(op)\
	    do{\
		int32_t b = stack.pop_as<int32_t>();\
		int32_t a = stack.pop_as<int32_t>();\
		stack.push_as<bool>(a op b);\
	    }while(false)\


		switch ((OpCode)advance())
		{
		case OpCode::CONSTANT:
		{
			uint8_t size = advance();
			uint8_t* constant = currentChunk->getConstant(advance());
			stack.push_sized(constant, size);
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
			*stack.peek_as_ref<int32_t>() *= -1;
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
			*stack.peek_as_ref<float>() *= -1;
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
			*stack.peek_as_ref<double>() *= -1;
			break;
		}
		case OpCode::BIT_NOT:
		{
			*stack.peek_as_ref<int32_t>() = ~(*stack.peek_as_ref<int32_t>());
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
			*stack.peek_as_ref<int32_t>() += 1;
			break;
		}
		case OpCode::IPRE_DEC:
		{
			*stack.peek_as_ref<int32_t>() -= 1;
			break;
		}
		case OpCode::IPOST_INC:
			break;
		case OpCode::IPOST_DEC:
			break;
		case OpCode::FPRE_INC:
		{
			*stack.peek_as_ref<float>() += 1;
			break;
		}
		case OpCode::FPRE_DEC:
		{
			*stack.peek_as_ref<float>() -= 1;
			break;
		}
		case OpCode::FPOST_INC:
			break;
		case OpCode::FPOST_DEC:
			break;
		case OpCode::DPRE_INC:
		{
			*stack.peek_as_ref<double>() += 1;
			break;
		}
		case OpCode::DPRE_DEC:
		{
			*stack.peek_as_ref<double>() -= 1;
			break;
		}
		case OpCode::DPOST_INC:
			break;
		case OpCode::DPOST_DEC:
			break;
		case OpCode::LOGIC_NOT:
		{
			*stack.peek_as_ref<bool>() = !(*stack.peek_as_ref<bool>());
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
			uint8_t* val = stack.peek_sized(size);
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
			stack.push_sized(val, size);
			break;
		}
		case OpCode::SET_LOCAL:
		{
			uint8_t slot = advance();
			uint8_t size = advance();
			stack.set_sized(stack.peek_sized(size), slot + this->frames.back().frameStart, size);

			break;
		}
		case OpCode::GET_LOCAL:
		{
			uint8_t slot = advance();
			uint8_t size = advance();
			stack.push_sized(stack.get_at_ref(slot + this->frames.back().frameStart), size);
			break;
		}
		case OpCode::SET_GLOBAL_POP:
		{
			size_t size = advance();
			size_t slot = advance();
			uint8_t* val = stack.pop_sized_ret(size);
			for (int i = 0; i < size; i++)
			{
				(*(uint8_t**)this->globals[slot])[i] = val[i];
			}
			delete[] val;
			break;
		}
		case OpCode::SET_LOCAL_POP:
		{
			uint8_t slot = advance();
			uint8_t size = advance();
			stack.set_sized(stack.peek_sized(size), slot + this->frames.back().frameStart, size);
			stack.pop_sized(size);
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
			if (!stack.pop_as<bool>())
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
			if (!stack.pop_as<bool>())
				this->ip += offset;
			break;
		}
		case OpCode::CALL:
		{
			uint8_t argSize = advance();
			Chunk* func = *(Chunk**)(stack.get_at_ref(this->stack.count() - sizeof(Chunk**) - argSize));
			this->frames.push_back(Frame(ip, currentChunk, this->stack.count() - argSize - sizeof(Chunk**)));
			this->currentChunk = func;
			this->ip = 0;
			break;
		}
		case OpCode::NATIVE_CALL:
		{
			uint8_t argSize = advance();
			NativeFunc* call = *(NativeFunc**)(stack.get_at_ref(this->stack.count() - sizeof(Chunk**) - argSize));
			NativeFn func = call->func;
			uint8_t* result = func(argSize, argSize > 0 ? (stack.get_at_ref(this->stack.count() - argSize)) : nullptr);

			this->stack.resize(this->stack.count() - argSize - sizeof(NativeFunc**));

			stack.push_sized(result, call->type.intrinsicType->getSize());
			break;
		}
		case OpCode::RETURN:
		{
			uint8_t size = advance();
			uint8_t* retVal = nullptr;

			auto frame = this->frames.back();
			this->frames.pop_back();

			if (size != 0)
			{
				stack.set_sized(stack.peek_sized(size), frame.frameStart, size);
			}
			this->stack.resize(frame.frameStart + size);

			this->ip = frame.ip;
			this->currentChunk = frame.chunk;

			if (size != 0)
				delete[] retVal;
			break;
		}
		}
	}
	return true;
}

void VM::typeCast(ValueType from, ValueType to)
{
#define CAST(value)   \
	switch (to)\
	{\
	case ValueType::INTEGER:\
		stack.push_as<int32_t>(value);\
		break;\
	case ValueType::FLOAT:\
		stack.push_as<float>(value);\
		break;\
	case ValueType::DOUBLE:\
		stack.push_as<double>(value);\
		break;\
	case ValueType::BOOL:\
		stack.push_as<bool>(value);\
		break;\
	case ValueType::CHAR:\
		stack.push_as<char>(value);\
		break;\
	}\

	switch (from)
	{
	case ValueType::INTEGER:
	{
		CAST(stack.pop_as<int32_t>());
		break;
	}
	case ValueType::FLOAT:
	{
		CAST(stack.pop_as<float>());
		break;
	}
	case ValueType::DOUBLE:
	{
		CAST(stack.pop_as<double>());
		break;
	}
	case ValueType::BOOL:
	{
		CAST(stack.pop_as<bool>());
		break;
	}
	case ValueType::CHAR:
	{
		CAST(stack.pop_as<char>());
		break;
	}
	}
}
