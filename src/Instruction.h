#pragma once
#include "Enviroment.hpp"

class InstVisitor;
class Instruction
{
public:
    virtual void accept(InstVisitor* visitor) = 0;
};

class InstConst : public Instruction
{
public:
    int id;

    InstConst(int id)
        : id(id) {}

    void accept(InstVisitor* visitor);
};

class InstCast : public Instruction
{
public:
    TypeTag from;
    TypeTag to;

    InstCast(TypeTag from, TypeTag to)
        : from(from), to(to) {}

    void accept(InstVisitor* visitor);
};

class InstAdd : public Instruction
{
public:
    TypeTag type;

    InstAdd(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstSub : public Instruction
{
public:
    TypeTag type;

    InstSub(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstMul : public Instruction
{
public:
    TypeTag type;

    InstMul(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstDiv : public Instruction
{
public:
    TypeTag type;

    InstDiv(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstNeg : public Instruction
{
public:
    TypeTag type;

    InstNeg(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstMod : public Instruction
{
public:
    InstMod()
    {
    }

    void accept(InstVisitor* visitor);
};

class InstBit : public Instruction
{
public:
    TokenType op_type;

    InstBit(TokenType op_type)
        : op_type(op_type) {}

    void accept(InstVisitor* visitor);
};

class InstNot : public Instruction
{
public:
    InstNot()
    {
    }

    void accept(InstVisitor* visitor);
};

class InstInc : public Instruction
{
public:
    TypeTag type;
    int inc;

    InstInc(TypeTag type, int inc)
        : type(type), inc(inc) {}

    void accept(InstVisitor* visitor);
};

class InstLess : public Instruction
{
public:
    TypeTag type;

    InstLess(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstLte : public Instruction
{
public:
    TypeTag type;

    InstLte(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstGreat : public Instruction
{
public:
    TypeTag type;

    InstGreat(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstGte : public Instruction
{
public:
    TypeTag type;

    InstGte(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstEq : public Instruction
{
public:
    TypeTag type;

    InstEq(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstNeq : public Instruction
{
public:
    TypeTag type;

    InstNeq(TypeTag type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstGetGlobal : public Instruction
{
public:
    std::string name;
    std::shared_ptr<Type> type;
    bool offset;

    InstGetGlobal(std::string name, std::shared_ptr<Type> type, bool offset = false)
        : name(name), type(type), offset(offset) {}

    void accept(InstVisitor* visitor);
};

class InstSetGlobal : public Instruction
{
public:
    std::string name;
    std::shared_ptr<Type> type;
    bool offset;

    InstSetGlobal(std::string name, std::shared_ptr<Type> type, bool offset = false)
        : name(name), type(type), offset(offset) {}

    void accept(InstVisitor* visitor);
};

class InstGetLocal : public Instruction
{
public:
    std::string name;
    Variable var;
    std::shared_ptr<Type> type;
    bool offset;

    InstGetLocal(std::string name, Variable var, std::shared_ptr<Type> type, bool offset = false)
        : name(name), var(var), type(type), offset(offset) {}

    void accept(InstVisitor* visitor);
};

class InstSetLocal : public Instruction
{
public:
    std::string name;
    Variable var;
    std::shared_ptr<Type> type;
    bool offset;

    InstSetLocal(std::string name, Variable var, std::shared_ptr<Type> type, bool offset = false)
        : name(name), var(var), type(type), offset(offset) {}

    void accept(InstVisitor* visitor);
};

class InstAlloc : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstAlloc(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstFree : public Instruction
{
public:
    InstFree() {}

    void accept(InstVisitor* visitor);
};

class InstGetDeref : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstGetDeref(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstSetDeref : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstSetDeref(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};


class InstGetDerefOff : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstGetDerefOff(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstSetDerefOff : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstSetDerefOff(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstCall : public Instruction
{
public:
    std::vector<std::shared_ptr<Type>> args;
    TypeTag callType;

    InstCall(std::vector<std::shared_ptr<Type>> args, TypeTag callType)
        : args(args), callType(callType) {}

    void accept(InstVisitor* visitor);
};

class InstPop : public Instruction
{
public:
    std::vector<std::shared_ptr<Type>> types;

    InstPop(std::vector<std::shared_ptr<Type>> types)
        : types(types) {}

    void accept(InstVisitor* visitor);
};

class InstPush : public Instruction
{
public:
    std::vector<std::shared_ptr<Type>> types;

    InstPush(std::vector<std::shared_ptr<Type>> types)
        : types(types) {}

    void accept(InstVisitor* visitor);
};

class InstReturn : public Instruction
{
public:
    std::shared_ptr<Type> type;

    InstReturn(std::shared_ptr<Type> type)
        : type(type) {}

    void accept(InstVisitor* visitor);
};

class InstLabel;

class InstJump : public Instruction
{
public:
    int pos;
    InstLabel* label;
    int type;

    InstJump(int pos, InstLabel* label, int type)
        : pos(pos), label(label), type(type) {}

    void accept(InstVisitor* visitor);
};

class InstLabel : public Instruction
{
public:
    int pos;
    size_t id;
    std::vector<InstJump*> patches;

    InstLabel(int pos, size_t id, std::vector<InstJump*> patches)
        : pos(pos), id(id), patches(patches) {}

    void accept(InstVisitor* visitor);
};
