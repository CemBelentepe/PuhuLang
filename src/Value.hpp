#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Scanner.h"

class IRChunk;
class Chunk;

enum class TypeTag
{
    BOOL,
    INTEGER,
    FLOAT,
    DOUBLE,
    CHAR,
    STRING,
    VOID,
    ARRAY,
    POINTER,
    FUNCTION,
    NATIVE,
    METHODE,
    CLASS,
    NULL_TYPE,
    AUTO,
    ERROR
};

class Type
{
public:
    TypeTag tag;
    std::shared_ptr<Type> intrinsicType;

    Type(TypeTag tag, std::shared_ptr<Type> intrinsicType)
        : tag(tag), intrinsicType(intrinsicType)
    {
    }

    virtual bool isPrimitive() = 0;
    virtual size_t getSize() = 0;
    virtual void print() = 0;
    virtual std::stringstream getName() = 0;
    virtual bool isSame(std::shared_ptr<Type> type) = 0;
};

// TODO: remove string and make it a allias for char[]& or char[]*

class TypePrimitive : public Type
{
public:
    TypePrimitive(TypeTag tag)
        : Type(tag, nullptr)
    {
    }

    bool isPrimitive()
    {
        return true;
    }

    size_t getSize()
    {
        return tag == TypeTag::VOID ? 0 : 1;
    }

    void print()
    {
        switch (tag)
        {
        case TypeTag::BOOL:
            std::cout << "bool";
            break;
        case TypeTag::INTEGER:
            std::cout << "int";
            break;
        case TypeTag::FLOAT:
            std::cout << "float";
            break;
        case TypeTag::DOUBLE:
            std::cout << "double";
            break;
        case TypeTag::CHAR:
            std::cout << "char";
            break;
        case TypeTag::VOID:
            std::cout << "void";
            break;
        case TypeTag::STRING:
            std::cout << "string";
            break;
        case TypeTag::NULL_TYPE:
            std::cout << "nl";
            break;
        case TypeTag::AUTO:
            std::cout << "auto";
            break;
        case TypeTag::ERROR:
            std::cout << "err";
            break;
        default:
            std::cout << "[ERROR] Unknown type: " << (int)tag << "\n";
            break;
        }
    }

    std::stringstream getName()
    {
        std::stringstream data;
        switch (tag)
        {
        case TypeTag::BOOL:
            data << "bool";
            break;
        case TypeTag::INTEGER:
            data << "int";
            break;
        case TypeTag::FLOAT:
            data << "float";
            break;
        case TypeTag::DOUBLE:
            data << "double";
            break;
        case TypeTag::CHAR:
            data << "char";
            break;
        case TypeTag::VOID:
            data << "void";
            break;
        case TypeTag::STRING:
            data << "string";
            break;
        case TypeTag::NULL_TYPE:
            data << "nl";
            break;
        case TypeTag::AUTO:
            data << "auto";
            break;
        case TypeTag::ERROR:
            data << "err";
            break;
        default:
            data << "[ERROR] Unknown type: " << (int)tag << "\n";
            break;
        }
        return data;
    }

    bool isSame(std::shared_ptr<Type> type)
    {
        return this->tag == type->tag && type->isPrimitive();
    }
};

class TypePointer : public Type
{
public:
    bool is_owner;
    TypePointer(bool is_owner, std::shared_ptr<Type> intrinsicType)
        : Type(TypeTag::POINTER, intrinsicType), is_owner(is_owner)
    {
    }

    bool isPrimitive()
    {
        return false;
    }

    size_t getSize()
    {
        return 1;
    }

    void print()
    {
        intrinsicType->print();
        std::cout << (is_owner ? "&" : "*");
    }

    std::stringstream getName()
    {
        auto ss = intrinsicType->getName();
        ss << (is_owner ? "&" : "*");
        return ss;
    }

    bool isSame(std::shared_ptr<Type> type)
    {
        if (this->tag == type->tag && ((TypePointer*)type.get())->is_owner == this->is_owner)
        {
            return this->intrinsicType->isSame(type->intrinsicType);
        }
        return false;
    }
};

class TypeArray : public Type
{
public:
    size_t size;
    TypeArray(size_t size, std::shared_ptr<Type> internalType)
        : Type(TypeTag::ARRAY, internalType), size(size)
    {
    }

    bool isPrimitive()
    {
        return false;
    }

    size_t getSize()
    {
        return size * intrinsicType->getSize();
    }

    void print()
    {
        intrinsicType->print();
        std::cout << "[]";
    }

    std::stringstream getName()
    {
        auto ss = intrinsicType->getName();
        ss << "[]";
        return ss;
    }
    bool isSame(std::shared_ptr<Type> type)
    {
        if (this->tag == type->tag && type.get()->getSize() == this->size)
        {
            return this->intrinsicType->isSame(type->intrinsicType);
        }
        return false;
    }
};

class TypeFunction : public Type
{
public:
    std::vector<std::shared_ptr<Type>> argTypes;
    bool is_variadic = false;

    TypeFunction(TypeTag funcType, std::shared_ptr<Type> retType, std::vector<std::shared_ptr<Type>> argTypes, bool is_variadic)
        : Type(funcType, retType), argTypes(argTypes), is_variadic(is_variadic) {}

    bool isPrimitive()
    {
        return false;
    }

    size_t getSize()
    {
        return 1;
    }

    void print()
    {
        intrinsicType->print();
        std::cout << "<(";
        for (int i = 0; i < argTypes.size(); i++)
        {
            if (i != 0)
                std::cout << ", ";
            argTypes[i]->print();
        }
        std::cout << ")";
    }

    std::stringstream getName()
    {
        auto ss = intrinsicType->getName();
        ss << "<(";
        for (int i = 0; i < argTypes.size(); i++)
        {
            if (i != 0)
                ss << ", ";
            ss << argTypes[i]->getName().str();
        }
        ss << ")";
        return ss;
    }

    bool isSame(std::shared_ptr<Type> type)
    {
        if (this->tag == type->tag)
        {
            bool argsSame = argTypes.size() == std::dynamic_pointer_cast<TypeFunction>(type)->argTypes.size();
            for (int i = 0; i < argTypes.size() && argsSame; i++)
            {
                argsSame = argTypes[i] == std::dynamic_pointer_cast<TypeFunction>(type);
            }

            return this->intrinsicType->isSame(type->intrinsicType) && argsSame;
        }
        return false;
    }
};

struct classMember
{
    std::shared_ptr<Type> type;
    Token name;
    size_t offset;
    
    classMember(){}

    classMember(std::shared_ptr<Type> type, Token name, size_t offset)
        : type(type), name(name), offset(offset) {}
};

class TypeClass : public Type
{
private:
    size_t size;
public:
    Token name;
    std::unordered_map<std::string, classMember> members;

    TypeClass(Token name)
        : Type(TypeTag::CLASS, nullptr), name(name), size(0)
    {
    }

    bool isPrimitive()
    {
        return false;
    }

    size_t getSize()
    {
        return size;
    }

    void print()
    {
        std::cout << "class " << name.getString();
    }

    std::stringstream getName()
    {
        std::stringstream ss;
        ss << "class " << name.getString();
        return ss;
    }

    bool isSame(std::shared_ptr<Type> type)
    {
        return type->tag == this->tag && ((TypeClass*)type.get())->name.getString() == name.getString();
    }

    void addMember(std::shared_ptr<Type> type, Token name)
    {
        if (members.find(name.getString()) == members.end())
        {
            size_t pos = size;
            size += type->getSize();
            members.insert({name.getString(), classMember(type, name, pos)});
        }
        else
        {
            std::cout << "[ERROR " << name.line << "] Member " << name.getString() << " is defined before.\n";
        }
    }
};

union Data
{
    bool valBool;
    char valChar;
    int32_t valInt;
    float valFloat;
    double valDouble;
    char* valString;
    Chunk* valChunk;
    std::vector<Data> (*valNative)(int, Data*);
    Data* valPtr;
};

typedef std::vector<Data> (*NativeFn)(int, Data*);

class Value
{
public:
    std::shared_ptr<Type> type;
    Data data;

    Value()
        : type(std::make_shared<TypePrimitive>(TypeTag::ERROR)), data({0})
    {
    }

    Value(std::shared_ptr<Type> type)
        : type(type), data({0})
    {
    }

    Value(bool value)
        : type(std::make_shared<TypePrimitive>(TypeTag::BOOL))
    {
        data.valBool = value;
    }

    Value(char value)
        : type(std::make_shared<TypePrimitive>(TypeTag::CHAR))
    {
        data.valChar = value;
    }

    Value(int32_t value)
        : type(std::make_shared<TypePrimitive>(TypeTag::INTEGER))
    {
        data.valInt = value;
    }

    Value(float value)
        : type(std::make_shared<TypePrimitive>(TypeTag::FLOAT))
    {
        data.valFloat = value;
    }

    Value(double value)
        : type(std::make_shared<TypePrimitive>(TypeTag::DOUBLE))
    {
        data.valDouble = value;
    }

    Value(const std::string& str)
        : type(std::make_shared<TypePrimitive>(TypeTag::STRING))
    {
        data.valString = new char[str.size() + 1];
        str.copy(data.valString, str.size(), 0);
        data.valString[str.size()] = 0;
    }

    Value(char* str)
        : type(std::make_shared<TypePrimitive>(TypeTag::STRING))
    {
        data.valString = str;
    }

    virtual ~Value() {}

    friend std::ostream& operator<<(std::ostream& os, const Value& val)
    {
        switch (val.type->tag)
        {
        case TypeTag::BOOL:
            os << val.data.valBool;
            break;
        case TypeTag::CHAR:
            os << val.data.valChar;
            break;
        case TypeTag::INTEGER:
            os << val.data.valInt;
            break;
        case TypeTag::FLOAT:
            os << val.data.valFloat << "f";
            break;
        case TypeTag::DOUBLE:
            os << val.data.valDouble << "lf";
            break;
        case TypeTag::STRING:
            os << val.data.valString;
            break;
        default:
            break;
        }

        return os;
    }

    bool operator==(const Value& right)
    {
        if (right.type != this->type)
            return false;

        switch (this->type->tag)
        {
        case TypeTag::BOOL:
            return this->data.valBool == right.data.valBool;
        case TypeTag::CHAR:
            return this->data.valChar == right.data.valChar;
        case TypeTag::INTEGER:
            return this->data.valInt == right.data.valInt;
        case TypeTag::FLOAT:
            return this->data.valFloat == right.data.valFloat;
        case TypeTag::DOUBLE:
            return this->data.valDouble == right.data.valDouble;
        case TypeTag::STRING:
            return std::strcmp(this->data.valString, right.data.valString) == 0;
        default:
            break;
        }

        return false;
    }

    bool operator!=(const Value& right)
    {
        return !(*this == right);
    }
};

class FuncValue : public Value
{
public:
    IRChunk* irChunk;

    FuncValue(std::shared_ptr<TypeFunction> type)
        : Value(type), irChunk(nullptr)
    {
    }

    virtual ~FuncValue() {}

    friend std::ostream& operator<<(std::ostream& os, const FuncValue& val)
    {
        os << val.irChunk;
        return os;
    }
};

class NativeFunc : public Value
{
public:
    bool is_variadic;

    NativeFunc(NativeFn func, std::shared_ptr<TypeFunction> type)
        : Value(type)
    {
        data.valNative = func;
    }

    virtual ~NativeFunc() {}
};
