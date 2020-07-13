#pragma once

#include "Instruction.h"
#include "Value.h"
#include <vector>


class IRChunk
{
private:
    std::vector<Instruction*> code;
    std::vector<Value*> constants;
public:
    IRChunk() {}
    ~IRChunk() {}

    inline size_t addConstant(Value* value)
    {
        for(size_t i = 0; i < constants.size(); i++)
            if(*constants[i] == *value)
                return i;

        constants.push_back(value);
        return constants.size() - 1;
    }

    inline Value* getConstant(size_t pos)
    {
        return constants[pos];
    }

    inline std::vector<Value*>& getConstants()
    {
        return this->constants;
    }

    inline void addCode(Instruction* inst)
    {
        code.push_back(inst);
    }

    inline std::vector<Instruction*>& getCode()
    {
        return this->code;
    }
};
