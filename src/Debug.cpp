#include "Debug.h"

void debugTokens(std::vector<Token> tokens)
{
    std::cout << "Tokens:\n";
    for (auto& token : tokens)
        std::cout << token << std::endl;
    std::cout << std::endl;
}

void debugAST(std::vector<Stmt*>& root)
{
    AstDebugger debugger;
    std::cout << "AST:\n";
    debugger.debugAll(root);
    std::cout << std::endl;
}

void debugInstructions(IRChunk* irChunk)
{
    InstDebugger debugger(irChunk);
    debugger.debugAll();
    std::cout << std::endl;
}

void printStack(std::vector<Data>& stack)
{
    std::cout << "\t";
    for (int i = 0; i < stack.size(); i++)
    {
        printf("[%.16X]", stack[i]);
    }
    std::cout << std::endl;
}

size_t printInstruction(const char* name, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << std::endl;
    return offset;
}

size_t printConstantInstruction(const char* name, Chunk* chunk, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << chunk->getConstant(chunk->code[++offset]).valChunk << std::endl;
    return offset;
}

size_t printPopInstruction(const char* name, Chunk* chunk, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << (unsigned int)chunk->code[++offset] << std::endl;
    return offset;
}

size_t printLocalInstruction(const char* name, Chunk* chunk, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t"
              << " " << (unsigned int)chunk->code[++offset] << std::endl;
    return offset;
}

size_t printLocalNInstruction(const char* name, Chunk* chunk, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t"
              << " " << (unsigned int)chunk->code[++offset] << " " << (unsigned int)chunk->code[++offset] << std::endl;
    return offset;
}

size_t printJumpInstruction(const char* name, Chunk* chunk, size_t offset, int dir)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << name << "\t" << dir * (int)(chunk->code[++offset]) + offset + 1 << std::endl;
    return offset;
}

size_t printCastInstruction(Chunk* chunk, size_t offset)
{
    std::cout << offset << "\t" << std::setw(10) << std::left << "CAST"
              << "      ";
    TypePrimitive t1((TypeTag)(chunk->code[++offset]));
    TypePrimitive t2((TypeTag)(chunk->code[++offset]));
    t1.print();
    std::cout << "->";
    t2.print();
    std::cout << std::endl;
    return offset;
}

size_t dissambleInstruction(Chunk* chunk, size_t offset)
{
    OpCode code = (OpCode)(chunk->code[offset]);
    std::cout << "\t";
    switch (code)
    {
    case OpCode::CONSTANT:
        return printConstantInstruction("CONSTANT", chunk, offset);
    case OpCode::IADD:
        return printInstruction("IADD", offset);
    case OpCode::ISUB:
        return printInstruction("ISUB", offset);
    case OpCode::IMUL:
        return printInstruction("IMUL", offset);
    case OpCode::IDIV:
        return printInstruction("IDIV", offset);
    case OpCode::INEG:
        return printInstruction("INEG", offset);
    case OpCode::MOD:
        return printInstruction("MOD", offset);
    case OpCode::FADD:
        return printInstruction("FADD", offset);
    case OpCode::FSUB:
        return printInstruction("FSUB", offset);
    case OpCode::FMUL:
        return printInstruction("FMUL", offset);
    case OpCode::FDIV:
        return printInstruction("FDIV", offset);
    case OpCode::FNEG:
        return printInstruction("FNEG", offset);
    case OpCode::DADD:
        return printInstruction("DADD", offset);
    case OpCode::DSUB:
        return printInstruction("DSUB", offset);
    case OpCode::DMUL:
        return printInstruction("DMUL", offset);
    case OpCode::DDIV:
        return printInstruction("DDIV", offset);
    case OpCode::DNEG:
        return printInstruction("DNEG", offset);
    case OpCode::BIT_NOT:
        return printInstruction("BIT_NOT", offset);
    case OpCode::BIT_AND:
        return printInstruction("BIT_AND", offset);
    case OpCode::BIT_OR:
        return printInstruction("BIT_OR", offset);
    case OpCode::BIT_XOR:
        return printInstruction("BIT_XOR", offset);
    case OpCode::BITSHIFT_LEFT:
        return printInstruction("BIT_LEFT", offset);
    case OpCode::BITSHIFT_RIGHT:
        return printInstruction("BIT_RIGHT", offset);
    case OpCode::IINC:
        return printInstruction("IINC", offset);
    case OpCode::IDEC:
        return printInstruction("IDEC", offset);
    case OpCode::FINC:
        return printInstruction("FINC", offset);
    case OpCode::FDEC:
        return printInstruction("FDEC", offset);
    case OpCode::DINC:
        return printInstruction("DINC", offset);
    case OpCode::DDEC:
        return printInstruction("DDEC", offset);
    case OpCode::LOGIC_NOT:
        return printInstruction("LOGIC_NOT", offset);
    case OpCode::DLESS:
        return printInstruction("DLESS", offset);
    case OpCode::DGREAT:
        return printInstruction("DGREAT", offset);
    case OpCode::DLESS_EQUAL:
        return printInstruction("DLESS_EQUAL", offset);
    case OpCode::DGREAT_EQUAL:
        return printInstruction("DGREAT_EQUAL", offset);
    case OpCode::DIS_EQUAL:
        return printInstruction("DIS_EQUAL", offset);
    case OpCode::DNOT_EQUAL:
        return printInstruction("DNOT_EQUAL", offset);
    case OpCode::ILESS:
        return printInstruction("ILESS", offset);
    case OpCode::IGREAT:
        return printInstruction("IGREAT", offset);
    case OpCode::ILESS_EQUAL:
        return printInstruction("ILESS_EQUAL", offset);
    case OpCode::IGREAT_EQUAL:
        return printInstruction("IGREAT_EQUAL", offset);
    case OpCode::IIS_EQUAL:
        return printInstruction("IIS_EQUAL", offset);
    case OpCode::INOT_EQUAL:
        return printInstruction("INOT_EQUAL", offset);
    case OpCode::CAST:
        return printCastInstruction(chunk, offset);
    case OpCode::POPN:
        return printPopInstruction("POPN", chunk, offset);
    case OpCode::PUSHN:
        return printPopInstruction("PUSH", chunk, offset);
    case OpCode::SET_GLOBAL:
        return printLocalInstruction("SET_GLOBAL", chunk, offset);
    case OpCode::GET_GLOBAL:
        return printLocalInstruction("GET_GLOBAL", chunk, offset);
    case OpCode::SET_LOCAL:
        return printLocalInstruction("SET_LOCAL", chunk, offset);
    case OpCode::GET_LOCAL:
        return printLocalInstruction("GET_LOCAL", chunk, offset);
    case OpCode::SET_GLOBALN:
        return printLocalNInstruction("SET_GLOBALN", chunk, offset);
    case OpCode::GET_GLOBALN:
        return printLocalNInstruction("GET_GLOBALN", chunk, offset);
    case OpCode::SET_LOCALN:
        return printLocalNInstruction("SET_LOCALN", chunk, offset);
    case OpCode::GET_LOCALN:
        return printLocalNInstruction("GET_LOCALN", chunk, offset);
    case OpCode::SET_GLOBAL_OFF:
        return printLocalInstruction("SET_GLOBAL_OFF", chunk, offset);
    case OpCode::GET_GLOBAL_OFF:
        return printLocalInstruction("GET_GLOBAL_OFF", chunk, offset);
    case OpCode::SET_LOCAL_OFF:
        return printLocalInstruction("SET_LOCAL_OFF", chunk, offset);
    case OpCode::GET_LOCAL_OFF:
        return printLocalInstruction("GET_LOCAL_OFF", chunk, offset);
    case OpCode::SET_GLOBAL_OFFN:
        return printLocalNInstruction("SET_GLOBAL_OFFN", chunk, offset);
    case OpCode::GET_GLOBAL_OFFN:
        return printLocalNInstruction("GET_GLOBAL_OFFN", chunk, offset);
    case OpCode::SET_LOCAL_OFFN:
        return printLocalNInstruction("SET_LOCAL_OFFN", chunk, offset);
    case OpCode::GET_LOCAL_OFFN:
        return printLocalNInstruction("GET_LOCAL_OFFN", chunk, offset);
    case OpCode::SET_GLOBAL_POP:
        return printLocalInstruction("SET_GLOBAL_POP", chunk, offset);
    case OpCode::SET_LOCAL_POP:
        return printLocalInstruction("SET_LOCAL_POP", chunk, offset);
    case OpCode::ALLOC:
        return printPopInstruction("ALLOC", chunk, offset);
    case OpCode::FREE:
        return printInstruction("FREE", offset);
    case OpCode::SET_DEREF:
        return printPopInstruction("SET_DEREF", chunk, offset);
    case OpCode::GET_DEREF:
        return printPopInstruction("GET_DEREF", chunk, offset);
    case OpCode::SET_DEREF_OFF:
        return printPopInstruction("SET_DEREF_OFF", chunk, offset);
    case OpCode::GET_DEREF_OFF:
        return printPopInstruction("GET_DEREF_OFF", chunk, offset);
    case OpCode::ADDR_LOCAL:
        return printLocalInstruction("ADDR_LOCAL", chunk, offset);
    case OpCode::ADDR_LOCAL_OFF:
        return printLocalInstruction("ADDR_LOCAL_OFF", chunk, offset);
    case OpCode::ADDR_GLOBAL:
        return printLocalInstruction("ADDR_GLOBAL", chunk, offset);
    case OpCode::ADDR_GLOBAL_OFF:
        return printLocalInstruction("ADDR_GLOBAL_OFF", chunk, offset);
    case OpCode::JUMP:
        return printJumpInstruction("JUMP", chunk, offset, 1);
    case OpCode::JUMP_NT_POP:
        return printJumpInstruction("JUMP_NT_POP", chunk, offset, 1);
    case OpCode::LOOP:
        return printJumpInstruction("LOOP", chunk, offset, -1);
    case OpCode::JUMP_NT:
        return printJumpInstruction("JUMP_NT", chunk, offset, 1);
    case OpCode::CALL:
        return printPopInstruction("CALL", chunk, offset);
    case OpCode::NATIVE_CALL:
        return printPopInstruction("NATIVE", chunk, offset);
    case OpCode::RETURN:
        return printPopInstruction("RETURN", chunk, offset);
    default:
        return printInstruction("UNKNOWN", offset);
    }
}

void dissambleChunk(Chunk* chunk)
{
    size_t offset = 0;
    while (offset < chunk->code.size())
    {
        offset = dissambleInstruction(chunk, offset) + 1;
    }
}