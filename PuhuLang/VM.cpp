#include "VM.h"

#ifdef _DEBUG
#include "Debug.h"
#endif // DEBUG_CODE_TRACE


VM::VM()
	:ip(0)
{}

VM::~VM()
{
	for (int i = 0; i < stack.size(); i++)
	{
		delete stack[i];
	}
}

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

	// #define BINARY_OP(op, type, dataType)\
	//     do{\
	//     Value* b = this->stack.back();\
	//     this->stack.pop_back();\
	//     Value* a = this->stack.back();\
	//     this->stack.pop_back();\
	//     type c = (*(type*)a->data) op (*(type*)b->data);\
	//     this->stack.push_back(new Value(new type(c), dataType));\
	//     }while(false)\

	switch (code)
	{
	case OpCode::CONSTANT:
	{
		Value* constant = currentChunk->getConstant(advance());
		push_to_stack(constant);
		break;
	}
	case OpCode::IADD:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt += b->data.valInt;
		delete b;
		break;
	}
	case OpCode::ISUB:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt -= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::IMUL:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt *= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::IDIV:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt /= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::INEG:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valInt *= -1;
		break;
	}
	case OpCode::MOD:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt %= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::FADD:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valFloat += b->data.valFloat;
		delete b;
		break;
	}
	case OpCode::FSUB:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valFloat -= b->data.valFloat;
		delete b;
		break;
	}
	case OpCode::FMUL:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valFloat *= b->data.valFloat;
		delete b;
		break;
	}
	case OpCode::FDIV:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valFloat /= b->data.valFloat;
		delete b;
		break;
	}
	case OpCode::FNEG:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valFloat *= -1;
		break;
	}
	case OpCode::DADD:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		((Value8b*)this->stack.back())->data.valDouble += b->data.valDouble;
		delete b;
		break;
	}
	case OpCode::DSUB:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		((Value8b*)this->stack.back())->data.valDouble -= b->data.valDouble;
		delete b;
		break;
	}
	case OpCode::DMUL:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		((Value8b*)this->stack.back())->data.valDouble *= b->data.valDouble;
		delete b;
		break;
	}
	case OpCode::DDIV:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		((Value8b*)this->stack.back())->data.valDouble /= b->data.valDouble;
		delete b;
		break;
	}
	case OpCode::DNEG:
	{
		Value8b* a = (Value8b*)(this->stack.back());
		a->data.valDouble *= -1;
		break;
	}
	case OpCode::BIT_NOT:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valInt = ~a->data.valInt;
		break;
	}
	case OpCode::BIT_AND:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt &= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::BIT_OR:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt |= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::BIT_XOR:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt ^= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::BITSHIFT_LEFT:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt <<= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::BITSHIFT_RIGHT:
	{
		Value4b* b = (Value4b*)this->stack.back();
		this->stack.pop_back();
		((Value4b*)this->stack.back())->data.valInt >>= b->data.valInt;
		delete b;
		break;
	}
	case OpCode::IPRE_INC:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valInt++;
		break;
	}
	case OpCode::IPRE_DEC:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valInt--;
		break;
	}
	case OpCode::IPOST_INC:
		break;
	case OpCode::IPOST_DEC:
		break;
	case OpCode::FPRE_INC:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valFloat++;
		break;
	}
	case OpCode::FPRE_DEC:
	{
		Value4b* a = (Value4b*)(this->stack.back());
		a->data.valFloat--;
		break;
	}
	case OpCode::FPOST_INC:
		break;
	case OpCode::FPOST_DEC:
		break;
	case OpCode::DPRE_INC:
	{
		Value8b* a = (Value8b*)(this->stack.back());
		a->data.valDouble++;
		break;
	}
	case OpCode::DPRE_DEC:
	{
		Value8b* a = (Value8b*)(this->stack.back());
		a->data.valDouble--;
		break;
	}
	case OpCode::DPOST_INC:
		break;
	case OpCode::DPOST_DEC:
		break;
	case OpCode::LOGIC_NOT:
	{
		Value1b* a = (Value1b*)(this->stack.back());
		a->data.valBool = !a->data.valBool;
		break;
	}
	case OpCode::LESS:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble < b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::GREAT:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble > b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::LESS_EQUAL:
	{

		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble <= b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::GREAT_EQUAL:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble >= b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::IS_EQUAL:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble == b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::NOT_EQUAL:
	{
		Value8b* b = (Value8b*)this->stack.back();
		this->stack.pop_back();
		Value8b* a = (Value8b*)this->stack.back();
		this->stack.pop_back();

		Value1b* c = new Value1b(a->data.valDouble != b->data.valDouble);
		this->stack.push_back(c);
		delete b;
		delete a;
		break;
	}
	case OpCode::CAST:
	{
		ValueType to = (ValueType)advance();
		Value* a = this->stack.back();
		this->stack.pop_back();
		this->stack.push_back(typeCast(a, to));
		delete a;
		break;
	}
	case OpCode::POP:
	{
		Value* a = (Value*)this->stack.back();
		this->stack.pop_back();
		delete a;
		break;
	}
	case OpCode::POPN:
	{
		int toPop = advance();
		for (int i = 0; i < toPop; i++)
		{
			delete this->stack.back();
			this->stack.pop_back();
		}
		break;
	}
	case OpCode::SET_GLOBAL:
	{
		std::string& name = ((StrValue*)(currentChunk->getConstant(advance())))->data;
		const Value* val = this->stack.back();
		this->globals[name]->setValue(val);
		break;
	}
	case OpCode::GET_GLOBAL:
	{
		std::string& name = ((StrValue*)(currentChunk->getConstant(advance())))->data;
		push_to_stack(this->globals[name]);
		break;
	}
	case OpCode::SET_LOCAL:
	{
		Value* newVal = this->stack.back();
		uint8_t slot = advance();
		Value* local = this->stack[slot + this->frames.back().frameStart];
		local->setValue(newVal);
		break;
	}
	case OpCode::GET_LOCAL:
	{
		uint8_t slot = advance();
		Value* val = this->stack[slot + this->frames.back().frameStart];
		push_to_stack(val);
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
		Value1b* cond = (Value1b*)this->stack.back();
		if (!cond->data.valBool)
			this->ip += offset;
		this->stack.pop_back();
		delete cond;
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
		Value1b* cond = (Value1b*)this->stack.back();
		if (!cond->data.valBool)
			this->ip += offset;
		break;
	}
	case OpCode::CALL:
	{
		uint8_t argc = advance();
		FuncValue* func = (FuncValue*)this->stack[this->stack.size() - 1 - argc];
		this->frames.push_back(Frame(ip, currentChunk, this->stack.size() - func->arity - 1));

		this->currentChunk = (Chunk*)(func->chunk);
		this->ip = 0;
		break;
	}
	case OpCode::NATIVE_CALL:
	{
		uint8_t argc = advance();
		NativeFn func = ((NativeFunc*)(stack[this->stack.size() - 1 - argc]))->func;
		Value* result = func(argc, argc > 0 ? (&(this->stack[this->stack.size() - argc])) : nullptr);

		for (int i = 0; i < argc + 1; i++)
		{
			if (this->stack.back()->type.type != ValueType::FUNCTION)
				delete this->stack.back();
			this->stack.pop_back();
		}

		this->stack.push_back(result);

		break;
	}
	case OpCode::RETURN:
	{
		Value* retVal = this->stack.back();
		this->stack.pop_back(); // pop return
		auto frame = this->frames.back();
		this->frames.pop_back();

		for (int i = this->stack.size() - 1; i > frame.frameStart; i--)
		{
			if (this->stack.back()->type.type != ValueType::FUNCTION)
				delete this->stack.back();
			this->stack.pop_back();
		}
		if (frame.frameStart != 0)
		{
			if (this->stack.back()->type.type != ValueType::FUNCTION)
				delete this->stack.back();
			this->stack.pop_back();
		}

		// this->stack.resize(frame.frameStart);

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
#define CAST(type, valType, valName)   \
    type data = ((valType*)val)->data.valName;\
	switch (to)\
	{\
	case ValueType::INTEGER:\
		ret = new Value4b((int32_t)data);\
		break;\
	case ValueType::FLOAT:\
		ret = new Value4b((float)data);\
		break;\
	case ValueType::DOUBLE:\
		ret = new Value8b((double)data);\
		break;\
	case ValueType::BOOL:\
		ret = new Value1b((bool)data);\
		break;\
	case ValueType::CHAR:\
		ret = new Value1b((char)data);\
		break;\
	default:\
		ret = new Value();\
		break;\
	}\

	Value* ret;

	switch (val->type.type)
	{
	case ValueType::INTEGER:
	{
		CAST(int, Value4b, valInt);
		break;
	}
	case ValueType::FLOAT:
	{
		CAST(float, Value4b, valFloat);
		break;
	}
	case ValueType::DOUBLE:
	{
		CAST(double, Value8b, valDouble);
		break;
	}
	case ValueType::BOOL:
	{
		CAST(bool, Value1b, valBool);
		break;
	}
	case ValueType::CHAR:
	{
		CAST(char, Value1b, valChar);
		break;
	}
	default:
	{
		ret = new Value();
		std::cout << "Casting error" << std::endl;
		break;
	}
	}

	return ret;
}

inline void VM::push_to_stack(Value* val)
{
	Value* ret;
	switch (val->type.type)
	{
	case ValueType::INTEGER:
		ret = new Value4b(((Value4b*)val)->data.valInt);
		break;
	case ValueType::FLOAT:
		ret = new Value4b(((Value4b*)val)->data.valFloat);
		break;
	case ValueType::DOUBLE:
		ret = new Value8b(((Value8b*)val)->data.valDouble);
		break;
	case ValueType::BOOL:
		ret = new Value1b(((Value1b*)val)->data.valBool);
		break;
	case ValueType::CHAR:
		ret = new Value1b(((Value1b*)val)->data.valChar);
		break;
	case ValueType::STRING:
		ret = new StrValue(((StrValue*)val)->data);
		break;
	case ValueType::NULL_TYPE:
		ret = new Value(ValueType::NULL_TYPE);
		break;
	case ValueType::VOID:
		ret = new Value(ValueType::VOID);
		break;
	case ValueType::FUNCTION:
		ret = val; // new FuncValue(((FuncValue*)val)->chunk, ((FuncValue*)val)->type.intrinsicType, ((FuncValue*)val)->arity);
		break;
	case ValueType::NATIVE:
		ret = new NativeFunc(((NativeFunc*)val)->func, ((NativeFunc*)val)->type.intrinsicType, ((NativeFunc*)val)->arity);
		break;
	default:
		std::cout << "Unknown type!";
		break;
	}

	this->stack.push_back(ret);
}
