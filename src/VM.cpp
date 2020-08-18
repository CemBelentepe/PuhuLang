#define DEBUG_CODE_TRACE

#include "VM.h"

#ifdef DEBUG_CODE_TRACE
#include "Debug.h"
#endif // DEBUG_CODE_TRACE

VM::VM(std::vector<Data> globals)
    : ip(0), currentChunk(nullptr), globals(globals)
{
#ifdef DEBUG_CODE_TRACE
    std::cout << "Globals: ";
    printStack(this->globals);
#endif
    stack.reserve(128);
}

VM::~VM()
{
}

bool VM::interpret(Chunk* entryChunk)
{
    this->currentChunk = entryChunk;
    while (ip < this->currentChunk->code.size())
    {
#ifdef DEBUG_CODE_TRACE
        printStack(this->stack);
        dissambleInstruction(currentChunk, this->ip);
#endif // DEBUG_CODE_TRACE

#define BINARY_OP(op, type)                         \
    do                                              \
    {                                               \
        auto b = stack.back().type;                 \
        stack.pop_back();                           \
        stack.back().type = stack.back().type op b; \
    } while (false)

#define CMP_OP(op, type)            \
    do                              \
    {                               \
        auto b = stack.back().type; \
        stack.pop_back();           \
        auto a = stack.back().type; \
        stack.pop_back();           \
        Data d;                     \
        d.valBool = a op b;         \
        stack.push_back(d);         \
    } while (false)

        switch ((OpCode)advance())
        {
        case OpCode::CONSTANT:
        {
            Data constant = currentChunk->getConstant(advance());
            stack.push_back(constant);
            break;
        }
        case OpCode::IADD:
        {
            BINARY_OP(+, valInt);
            break;
        }
        case OpCode::ISUB:
        {
            BINARY_OP(-, valInt);
            break;
        }
        case OpCode::IMUL:
        {
            BINARY_OP(*, valInt);
            break;
        }
        case OpCode::IDIV:
        {
            BINARY_OP(/, valInt);
            break;
        }
        case OpCode::INEG:
        {
            stack.back().valInt *= -1;
            break;
        }
        case OpCode::MOD:
        {
            BINARY_OP(%, valInt);
            break;
        }
        case OpCode::FADD:
        {
            BINARY_OP(+, valFloat);
            break;
        }
        case OpCode::FSUB:
        {
            BINARY_OP(-, valFloat);
            break;
        }
        case OpCode::FMUL:
        {
            BINARY_OP(*, valFloat);
            break;
        }
        case OpCode::FDIV:
        {
            BINARY_OP(/, valFloat);
            break;
        }
        case OpCode::FNEG:
        {
            stack.back().valFloat *= -1;
            break;
        }
        case OpCode::DADD:
        {
            BINARY_OP(+, valDouble);
            break;
        }
        case OpCode::DSUB:
        {
            BINARY_OP(-, valDouble);
            break;
        }
        case OpCode::DMUL:
        {
            BINARY_OP(*, valDouble);
            break;
        }
        case OpCode::DDIV:
        {
            BINARY_OP(/, valDouble);
            break;
        }
        case OpCode::DNEG:
        {
            stack.back().valDouble *= -1;
            break;
        }
        case OpCode::BIT_NOT:
        {
            stack.back().valInt = ~stack.back().valInt;
            break;
        }
        case OpCode::BIT_AND:
        {
            BINARY_OP(&, valInt);
            break;
        }
        case OpCode::BIT_OR:
        {
            BINARY_OP(|, valInt);
            break;
        }
        case OpCode::BIT_XOR:
        {
            BINARY_OP(^, valInt);
            break;
        }
        case OpCode::BITSHIFT_LEFT:
        {
            BINARY_OP(<<, valInt);
            break;
        }
        case OpCode::BITSHIFT_RIGHT:
        {
            BINARY_OP(>>, valInt);
            break;
        }
        case OpCode::IINC:
        {
            stack.back().valInt++;
            break;
        }
        case OpCode::IDEC:
        {
            stack.back().valInt--;
            break;
        }
        case OpCode::FINC:
        {
            stack.back().valFloat++;
            break;
        }
        case OpCode::FDEC:
        {
            stack.back().valFloat--;
            break;
        }
        case OpCode::DINC:
        {
            stack.back().valDouble++;
            break;
        }
        case OpCode::DDEC:
        {
            stack.back().valDouble--;
            break;
        }
        case OpCode::LOGIC_NOT:
        {
            stack.back().valBool = !stack.back().valBool;
            break;
        }
        case OpCode::DLESS:
        {
            CMP_OP(<, valDouble);
            break;
        }
        case OpCode::DGREAT:
        {
            CMP_OP(>, valDouble);
            break;
        }
        case OpCode::ILESS:
        {
            CMP_OP(<, valInt);
            break;
        }
        case OpCode::IGREAT:
        {
            CMP_OP(>, valInt);
            break;
        }
        case OpCode::DLESS_EQUAL:
        {
            CMP_OP(<=, valDouble);
            break;
        }
        case OpCode::DGREAT_EQUAL:
        {
            CMP_OP(>=, valDouble);
            break;
        }
        case OpCode::DIS_EQUAL:
        {
            CMP_OP(==, valDouble);
            break;
        }
        case OpCode::DNOT_EQUAL:
        {
            CMP_OP(!=, valDouble);
            break;
        }
        case OpCode::ILESS_EQUAL:
        {
            CMP_OP(<=, valInt);
            break;
        }
        case OpCode::IGREAT_EQUAL:
        {
            CMP_OP(>=, valInt);
            break;
        }
        case OpCode::IIS_EQUAL:
        {
            CMP_OP(==, valInt);
            break;
        }
        case OpCode::INOT_EQUAL:
        {
            CMP_OP(!=, valInt);
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
            uint8_t toPop = advance();
            stack.resize(stack.size() - toPop);
            break;
        }
        case OpCode::PUSHN:
        {
            uint8_t toPop = advance();
            stack.resize(stack.size() + toPop);
            break;
        }
        case OpCode::SET_GLOBAL:
        {
            uint8_t slot = advance();
            Data val = stack.back();
            globals[slot] = val;
            break;
        }
        case OpCode::GET_GLOBAL:
        {
            uint8_t slot = advance();
            stack.push_back(globals[slot]);
            break;
        }
        case OpCode::SET_LOCAL:
        {
            uint8_t slot = advance();
            stack[slot + this->frames.back().frameStart] = stack.back();
            break;
        }
        case OpCode::GET_LOCAL:
        {
            uint8_t slot = advance();
            stack.push_back(stack[slot + this->frames.back().frameStart]);
            break;
        }
        case OpCode::SET_GLOBALN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            memcpy(&globals[slot], &stack[stack.size() - size], size * sizeof(Data));
            break;
        }
        case OpCode::GET_GLOBALN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            uint8_t pos = stack.size();
            stack.resize(pos + size);
            memcpy(&stack[pos], &globals[slot], size * sizeof(Data));
            break;
        }
        case OpCode::SET_LOCALN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            memcpy(&stack[slot + this->frames.back().frameStart], &stack[stack.size() - size], size * sizeof(Data));
            break;
        }
        case OpCode::GET_LOCALN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            uint8_t pos = stack.size();
            stack.resize(pos + size);
            memcpy(&stack[pos], &stack[slot + this->frames.back().frameStart], size * sizeof(Data));
            break;
        }
        case OpCode::SET_GLOBAL_OFF:
        {
            uint8_t slot = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            Data val = stack.back();
            globals[slot + offset] = val;
            break;
        }
        case OpCode::GET_GLOBAL_OFF:
        {
            uint8_t slot = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            stack.push_back(globals[slot + offset]);
            break;
        }
        case OpCode::SET_LOCAL_OFF:
        {
            uint8_t slot = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            stack[slot + offset + this->frames.back().frameStart] = stack.back();
            break;
        }
        case OpCode::GET_LOCAL_OFF:
        {
            uint8_t slot = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            stack.push_back(stack[slot + offset + this->frames.back().frameStart]);
            break;
        }
        case OpCode::SET_GLOBAL_OFFN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            memcpy(&globals[slot + offset], &stack[stack.size() - size], size * sizeof(Data));
            break;
        }
        case OpCode::GET_GLOBAL_OFFN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            memcpy(&stack[slot + this->frames.back().frameStart + offset], &stack[stack.size() - size], size * sizeof(Data));
            break;
        }
        case OpCode::SET_LOCAL_OFFN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            memcpy(&stack[slot + this->frames.back().frameStart + offset], &stack[stack.size() - size], size * sizeof(Data));
            break;
        }
        case OpCode::GET_LOCAL_OFFN:
        {
            uint8_t slot = advance();
            uint8_t size = advance();
            int32_t offset = stack.back().valInt;
            stack.pop_back();
            uint8_t pos = stack.size();
            stack.resize(pos + size);
            memcpy(&stack[pos], &stack[slot + this->frames.back().frameStart + offset], size * sizeof(Data));
            break;
        }
        case OpCode::SET_GLOBAL_POP:
        {
            uint8_t slot = advance();
            Data val = stack.back();
            stack.pop_back();
            globals.push_back(val);
            break;
        }
        case OpCode::SET_LOCAL_POP:
        {
            uint8_t slot = advance();
            stack[slot + this->frames.back().frameStart] = stack.back();
            stack.pop_back();
            break;
        }
        case OpCode::ALLOC:
        {
            uint8_t size = advance();
            Data data;
            data.valPtr = new Data[size];
            stack.push_back(data);
            break;
        }
        case OpCode::FREE:
        {
            Data ptr = stack.back();
            stack.pop_back();
            delete[] ptr.valPtr;
            break;
        }
        case OpCode::SET_DEREF:
        {
            uint8_t size = advance();
            Data ptr = stack.back();
            stack.pop_back();
            // TODO: optimize
            for (int i = 0; i < size; i++)
            {
                ptr.valPtr[i] = stack.back();
                stack[stack.size() - i];
            }
            break;
        }
        case OpCode::GET_DEREF:
        {
            uint8_t size = advance();
            Data ptr = stack.back();
            stack.pop_back();
            for (int i = 0; i < size; i++)
                stack.push_back(ptr.valPtr[i]);
            break;
        }
        case OpCode::SET_DEREF_OFF:
        {
            uint8_t size = advance();
            Data offset = stack.back();
            stack.pop_back();
            Data ptr = stack.back();
            stack.pop_back();
            // TODO: optimize
            for (int i = 0; i < size; i++)
            {
                ptr.valPtr[i+offset.valInt] = stack.back();
                stack[stack.size() - i];
            }
            break;
        }
        case OpCode::GET_DEREF_OFF:
        {
            uint8_t size = advance();
            Data offset = stack.back();
            stack.pop_back();
            Data ptr = stack.back();
            stack.pop_back();
            for (int i = 0; i < size; i++)
                stack.push_back(ptr.valPtr[i+offset.valInt]);
            break;
        }
        case OpCode::ADDR_LOCAL:
        {
            uint8_t slot = advance();
            Data addr;
            addr.valPtr = &stack[slot + this->frames.back().frameStart];
            stack.push_back(addr);
            break;
        }
        case OpCode::ADDR_GLOBAL:
        {
            uint8_t slot = advance();
            Data addr;
            addr.valPtr = &globals[slot];
            stack.push_back(addr);
            break;
        }
        case OpCode::ADDR_LOCAL_OFF:
        {
            uint8_t slot = advance();
            Data offset = stack.back();
            stack.pop_back();
            Data addr;
            addr.valPtr = &stack[slot + this->frames.back().frameStart + offset.valInt];
            stack.push_back(addr);
            break;
        }
        case OpCode::ADDR_GLOBAL_OFF:
        {
            uint8_t slot = advance();
            Data offset = stack.back();
            stack.pop_back();
            Data addr;
            addr.valPtr = &globals[slot + offset.valInt];
            stack.push_back(addr);
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
            if (!stack.back().valBool)
                this->ip += offset;
            stack.pop_back();
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
            if (!stack.back().valBool)
                this->ip += offset;
            break;
        }
        case OpCode::CALL:
        {
            uint8_t argSize = advance();
            Chunk* func = stack.back().valChunk;
            stack.pop_back();
            this->frames.push_back(Frame(ip, currentChunk, this->stack.size() - argSize));
            this->currentChunk = func;
            this->ip = 0;
            break;
        }
        case OpCode::NATIVE_CALL:
        {
            uint8_t argSize = advance();
            NativeFn func = stack.back().valNative;
            stack.pop_back();
            std::vector<Data> result = func(argSize, argSize > 0 ? &stack[this->stack.size() - argSize] : nullptr);
            this->stack.resize(this->stack.size() - argSize);
            for(int i = 0; i < result.size(); i++)
                stack.push_back(result[i]);
            break;
        }
        case OpCode::RETURN:
        {
            uint8_t size = advance();

            auto frame = this->frames.back();
            this->frames.pop_back();

            if (size == 1)
            {
                stack[frame.frameStart] = stack.back();
            }
            else if (size != 0)
            {
                memcpy(&stack[frame.frameStart], &stack[stack.size() - size], size * sizeof(Data));
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
#define CAST(value)          \
    Data d;                  \
    switch (to)              \
    {                        \
    case TypeTag::INTEGER:   \
        d.valInt = value;    \
        break;               \
    case TypeTag::FLOAT:     \
        d.valFloat = value;  \
        break;               \
    case TypeTag::DOUBLE:    \
        d.valDouble = value; \
        break;               \
    case TypeTag::BOOL:      \
        d.valBool = value;   \
        break;               \
    case TypeTag::CHAR:      \
        d.valChar = value;   \
        break;               \
    }                        \
    stack.push_back(d);

    Data data = stack.back();
    stack.pop_back();

    switch (from)
    {
    case TypeTag::INTEGER:
    {
        CAST(data.valInt);
        break;
    }
    case TypeTag::FLOAT:
    {
        CAST(data.valFloat);
        break;
    }
    case TypeTag::DOUBLE:
    {
        CAST(data.valDouble);
        break;
    }
    case TypeTag::BOOL:
    {
        CAST(data.valBool);
        break;
    }
    case TypeTag::CHAR:
    {
        CAST(data.valChar);
        break;
    }
    }
}