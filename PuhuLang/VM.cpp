#include "Common.h"
#include "VM.h"
#ifdef DEBUG_CODE_TRACE
#include "Debug.h"
#endif // DEBUG_CODE_TRACE


VM::VM()
	:ip(0)
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

#define BINARY_OP(op, type, dataType)\
    do{\
    Value* b = this->stack.back();\
    this->stack.pop_back();\
    Value* a = this->stack.back();\
    this->stack.pop_back();\
    type c = (*(type*)a->data) op (*(type*)b->data);\
    this->stack.push_back(new Value(new type(c), dataType));\
    }while(false)\

	switch (code)
	{
	case OpCode::CONSTANT:
	{
		Value* constant = currentChunk->getConstant(advance());
		this->stack.push_back(constant);
		break;
	}
	case OpCode::IADD:
	{
		BINARY_OP(+, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::ISUB:
	{
		BINARY_OP(-, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::IMUL:
	{
		BINARY_OP(*, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::IDIV:
	{
		BINARY_OP(/ , int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::INEG:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		int b = -(*((int*)a->data));
		this->stack.push_back(new Value(new int(b), ValueType::INTEGER));
		break;
	}
	case OpCode::MOD:
	{
		BINARY_OP(%, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::FADD:
	{
		BINARY_OP(+, float, ValueType::FLOAT);
		break;
	}
	case OpCode::FSUB:
	{
		BINARY_OP(-, float, ValueType::FLOAT);
		break;
	}
	case OpCode::FMUL:
	{
		BINARY_OP(*, float, ValueType::FLOAT);
		break;
	}
	case OpCode::FDIV:
	{
		BINARY_OP(/ , float, ValueType::FLOAT);
		break;
	}
	case OpCode::FNEG:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		float b = -(*((float*)a->data));
		this->stack.push_back(new Value(new float(b), ValueType::FLOAT));
		break;
	}
	case OpCode::DADD:
	{
		BINARY_OP(+, double, ValueType::DOUBLE);
		break;
	}
	case OpCode::DSUB:
	{
		BINARY_OP(-, double, ValueType::DOUBLE);
		break;
	}
	case OpCode::DMUL:
	{
		BINARY_OP(*, double, ValueType::DOUBLE);
		break;
	}
	case OpCode::DDIV:
	{
		BINARY_OP(/, double, ValueType::DOUBLE);
		break;
	}
	case OpCode::DNEG:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		double b = -(*((double*)a->data));
		this->stack.push_back(new Value(new double(b), ValueType::DOUBLE));
		break;
	}
	case OpCode::BIT_NOT:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		int32_t b = ~(*((int32_t*)a->data));
		this->stack.push_back(new Value(new int32_t(b), ValueType::INTEGER));
		break;
	}
	case OpCode::BIT_AND:
	{
		BINARY_OP(&, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::BIT_OR:
	{
		BINARY_OP(|, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::BIT_XOR:
	{
		BINARY_OP(^, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::BITSHIFT_LEFT:
	{
		BINARY_OP(<< , int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::BITSHIFT_RIGHT:
	{
		BINARY_OP(>>, int32_t, ValueType::INTEGER);
		break;
	}
	case OpCode::IPRE_INC:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		int32_t b = (*((int32_t*)a->data)) + 1;
		this->stack.push_back(new Value(new int32_t(b), ValueType::INTEGER));
		break;
	}
	case OpCode::IPRE_DEC:
	{

		Value* a = this->stack.back();
		this->stack.pop_back();
		int32_t b = (*((int32_t*)a->data)) - 1;
		this->stack.push_back(new Value(new int32_t(b), ValueType::INTEGER));
		break;
	}
	case OpCode::IPOST_INC:
		break;
	case OpCode::IPOST_DEC:
		break;
	case OpCode::FPRE_INC:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		float b = (*((float*)a->data)) + 1;
		this->stack.push_back(new Value(new float(b), ValueType::FLOAT));
		break;
	}
	case OpCode::FPRE_DEC:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		float b = (*((float*)a->data)) - 1;
		this->stack.push_back(new Value(new float(b), ValueType::FLOAT));
		break;
	}
	case OpCode::FPOST_INC:
		break;
	case OpCode::FPOST_DEC:
		break;
	case OpCode::DPRE_INC:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		double b = (*((double*)a->data)) + 1;
		this->stack.push_back(new Value(new double(b), ValueType::DOUBLE));
		break;
	}
	case OpCode::DPRE_DEC:
	{
		Value* a = this->stack.back();
		this->stack.pop_back();
		double b = (*((double*)a->data)) - 1;
		this->stack.push_back(new Value(new double(b), ValueType::DOUBLE));
		break;
	}
	case OpCode::DPOST_INC:
		break;
	case OpCode::DPOST_DEC:
		break;
	case OpCode::LOGIC_NOT:
	{

		Value* a = this->stack.back();
		this->stack.pop_back();
		double b = !(*((double*)a->data));
		this->stack.push_back(new Value(new double(b), ValueType::DOUBLE));
		break;
	}
	case OpCode::LOGIC_AND:
	{
		BINARY_OP(&&, bool, ValueType::BOOL);
		break;
	}
	case OpCode::LOGIC_OR:
	{
		BINARY_OP(|| , bool, ValueType::BOOL);
		break;
	}
	case OpCode::LESS:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) < (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::GREAT:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) > (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::LESS_EQUAL:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) <= (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::GREAT_EQUAL:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) >= (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::IS_EQUAL:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) == (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::NOT_EQUAL:
	{
		Value* b = this->stack.back();
		this->stack.pop_back();
		Value* a = this->stack.back();
		this->stack.pop_back();
		bool c = (*(double*)a->data) != (*(double*)b->data);
		this->stack.push_back(new Value(new bool(c), ValueType::BOOL));
		break;
	}
	case OpCode::CAST:
	{
		ValueType to = (ValueType)advance();
		Value* a = new Value(*this->stack.back());
		this->stack.pop_back();
		this->stack.push_back(typeCast(a, to));
		break;
	}
	case OpCode::POP:
	{
		this->stack.pop_back();
		break;
	}
	case OpCode::POPN:
	{
		int toPop = advance();
		this->stack.resize(this->stack.size() - toPop);
		break;
	}
	case OpCode::SET_GLOBAL:
	{
		std::string* name = (std::string*)(currentChunk->getConstant(advance())->data);
		Value* val = this->stack.back();
		this->globals[*name]->data = val->data;
		break;
	}
	case OpCode::GET_GLOBAL:
	{
		std::string* name = (std::string*)(currentChunk->getConstant(advance())->data);
		this->stack.push_back(this->globals[*name]);
		break;
	}
	case OpCode::SET_LOCAL:
	{
		Value* newVal = this->stack.back();
		uint8_t slot = advance();
		Value* local = this->stack[slot + this->frames.back().frameStart];
		local->setData(newVal->data);
		break;
	}
	case OpCode::GET_LOCAL:
	{
		uint8_t slot = advance();
		Value* val = this->stack[slot + this->frames.back().frameStart];
		this->stack.push_back(val);
		break;
	}
	case OpCode::JUMP:
	{
		uint8_t offset = advance();
		this->ip += offset;
		break;
	}
	case OpCode::JUMP_NT:
	{
		uint8_t offset = advance();
		Value* cond = this->stack.back();
		this->stack.pop_back();
		if(!(*(bool*)(cond->data)))
			this->ip += offset;
		break;
	}
	case OpCode::LOOP:
	{
		uint8_t offset = advance();
		this->ip -= offset;
		break;
	}
	case OpCode::CALL:
	{
		uint8_t argc = advance();
		FuncValue* func = (FuncValue*)this->stack[this->stack.size() - 1 - argc];
		this->frames.push_back(Frame(ip, currentChunk, this->stack.size() - func->arity - 1));

		this->currentChunk = (Chunk*)(func->data);
		this->ip = 0;
		break;
	}
	case OpCode::NATIVE_CALL:
	{
		uint8_t argc = advance();
		NativeFn func = (NativeFn)(stack[this->stack.size() - 1 - argc]->data);
		Value* result = func(argc, argc > 0 ? (&(this->stack[this->stack.size() - argc])) : nullptr);
		this->stack.resize(this->stack.size() - argc - 1);
		if (result->type != ValueType::VOID)
			this->stack.push_back(result);
		break;
	}
	case OpCode::RETURN:
	{
		Value* retVal = this->stack.back();
		this->stack.pop_back(); // pop return
		auto frame = this->frames.back();
		this->frames.pop_back();

		this->stack.resize(frame.frameStart);

		this->ip = frame.ip;
		this->currentChunk = frame.chunk;
		this->stack.push_back(retVal);
		break;
	}
	}
}

uint8_t VM::advance()
{
	return this->currentChunk->code[this->ip++];
}

Value* VM::typeCast(Value* val, ValueType to)
{
#define CAST(y)   \
    switch (to)\
    {\
    case ValueType::INTEGER:\
        val->data = new int32_t(*((y*)val->data));\
        break;\
    case ValueType::FLOAT:\
        val->data = new float(*((y*)val->data));\
        break;\
    case ValueType::DOUBLE:\
        val->data = new double(*((y*)val->data));\
        break;\
    case ValueType::BOOL:\
        val->data = new bool(*((y*)val->data));\
        break;\
    case ValueType::CHAR:\
        val->data = new char(*((y*)val->data));\
        break;\
    default:\
        val->data = nullptr;\
        break;\
    }\


	switch (val->type.type)
	{
	case ValueType::INTEGER:
	{
		CAST(int);
		break;
	}
	case ValueType::FLOAT:
	{
		CAST(float);
		break;
	}
	case ValueType::DOUBLE:
	{
		CAST(double);
		break;
	}
	case ValueType::BOOL:
	{
		CAST(bool);
		break;
	}
	case ValueType::CHAR:
	{
		CAST(char);
		break;
	}
	default:
	{
		CAST(char);
		break;
	}
	}

	val->type = to;
	return val;
}
