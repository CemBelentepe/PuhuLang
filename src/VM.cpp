// #define DEBUG_CODE_TRACE

#include "VM.h"

#ifdef DEBUG_CODE_TRACE
#include "Debug.h"
#endif // DEBUG_CODE_TRACE

VM::VM(ArrayList<uint8_t> globals)
	:ip(0), currentChunk(nullptr), globals(globals)
{
#ifdef DEBUG_CODE_TRACE
	std::cout << "Globals: ";
	printStack(this->globals);
#endif
}

VM::~VM()
{}

bool VM::interpret(Chunk* entryChunk)
{
	this->currentChunk = entryChunk;
	while (ip < this->currentChunk->code.size())
	{
#ifdef DEBUG_CODE_TRACE
		printStack(this->stack);
		dissambleInstruction(currentChunk, this->ip);
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
		case OpCode::IINC:
		{
			*stack.peek_as_ref<int32_t>() += 1;
			break;
		}
		case OpCode::IDEC:
		{
			*stack.peek_as_ref<int32_t>() -= 1;
			break;
		}
		case OpCode::FINC:
		{
			*stack.peek_as_ref<float>() += 1;
			break;
		}
		case OpCode::FDEC:
		{
			*stack.peek_as_ref<float>() -= 1;
			break;
		}
		case OpCode::DINC:
		{
			*stack.peek_as_ref<double>() += 1;
			break;
		}
		case OpCode::DDEC:
		{
			*stack.peek_as_ref<double>() -= 1;
			break;
		}
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
			TypeTag from = (TypeTag)advance();
			TypeTag to = (TypeTag)advance();
			typeCast(from, to);
			break;
		}
		case OpCode::POPN:
		{
			int toPop = advance();
			stack.pop_sized(toPop);
			break;
		}
		case OpCode::SET_GLOBAL:
		{
			size_t size = advance();
			size_t slot = advance();
			uint8_t* val = stack.peek_sized(size);
			globals.set_sized(val, slot, size);
			break;
		}
		case OpCode::GET_GLOBAL:
		{
			size_t size = advance();
			size_t slot = advance();
			stack.push_sized(globals.get_at_ref(slot), size);
			break;
		}
		case OpCode::SET_LOCAL:
		{
			uint8_t size = advance();
			uint8_t slot = advance();
			stack.set_sized(stack.peek_sized(size), slot + this->frames.back().frameStart, size);

			break;
		}
		case OpCode::GET_LOCAL:
		{
			uint8_t size = advance();
			uint8_t slot = advance();
			stack.push_sized(stack.get_at_ref(slot + this->frames.back().frameStart), size);
			break;
		}
		case OpCode::SET_GLOBAL_POP:
		{
			size_t size = advance();
			size_t slot = advance();
			uint8_t* val = stack.pop_sized_ret(size);
			globals.set_sized(val, slot, size);
			delete[] val;
			break;
		}
		case OpCode::SET_LOCAL_POP:
		{
			uint8_t size = advance();
			uint8_t slot = advance();
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
			Chunk* func = stack.pop_as<Chunk*>();
			this->frames.push_back(Frame(ip, currentChunk, this->stack.count() - argSize));
			this->currentChunk = func;
			this->ip = 0;
			break;
		}
		case OpCode::NATIVE_CALL:
		{
			uint8_t argSize = advance();
			NativeFunc* call = stack.pop_as<NativeFunc*>();
			NativeFn func = call->data.valNative;
			uint8_t* result = func(argSize, argSize > 0 ? (stack.get_at_ref(this->stack.count() - argSize)) : nullptr);
			this->stack.resize(this->stack.count() - argSize);
			stack.push_sized(result, call->type.intrinsicType->getSize());
			break;
		}
		case OpCode::RETURN:
		{
			uint8_t size = advance();

			auto frame = this->frames.back();
			this->frames.pop_back();

			if (size != 0)
			{
				stack.set_sized(stack.peek_sized(size), frame.frameStart, size);
			}
			this->stack.resize(frame.frameStart + size);

			this->ip = frame.ip;
			this->currentChunk = frame.chunk;

			break;
		}
		}
	}

#ifdef DEBUG_CODE_TRACE
	printStack(this->stack);
#endif // DEBUG_CODE_TRACE

	return true;
}

void VM::typeCast(TypeTag from, TypeTag to)
{
#define CAST(value)   \
	switch (to)\
	{\
	case TypeTag::INTEGER:\
		stack.push_as<int32_t>(value);\
		break;\
	case TypeTag::FLOAT:\
		stack.push_as<float>(value);\
		break;\
	case TypeTag::DOUBLE:\
		stack.push_as<double>(value);\
		break;\
	case TypeTag::BOOL:\
		stack.push_as<bool>(value);\
		break;\
	case TypeTag::CHAR:\
		stack.push_as<char>(value);\
		break;\
	}\

	switch (from)
	{
	case TypeTag::INTEGER:
	{
		CAST(stack.pop_as<int32_t>());
		break;
	}
	case TypeTag::FLOAT:
	{
		CAST(stack.pop_as<float>());
		break;
	}
	case TypeTag::DOUBLE:
	{
		CAST(stack.pop_as<double>());
		break;
	}
	case TypeTag::BOOL:
	{
		CAST(stack.pop_as<bool>());
		break;
	}
	case TypeTag::CHAR:
	{
		CAST(stack.pop_as<char>());
		break;
	}
	}
}