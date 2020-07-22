#pragma once

#include <cstring>
#include <iostream>
#include <memory>
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
    USER_DEFINED,
    NULL_TYPE,
    ERROR
};

class Type
{
public:
    TypeTag tag;
    std::shared_ptr<Type> intrinsicType;

    Type()
        : tag(TypeTag::ERROR), intrinsicType(nullptr)
    {
    }

    Type(TypeTag tag)
        : tag(tag), intrinsicType(nullptr)
    {
    }

    Type(std::shared_ptr<Type> dataType)
    {
        if (dataType)
        {
            tag = dataType->tag;
            if (dataType->intrinsicType)
            {
                intrinsicType = dataType->intrinsicType;
            }
        }
    }

    Type(TypeTag tag, std::shared_ptr<Type> intrinsicType)
        : tag(tag), intrinsicType(intrinsicType)
    {
    }

    inline bool operator==(const Type& type) const
    {
        if (this->tag == type.tag)
            return this->intrinsicType == type.intrinsicType;
        else
            return false;
    }
    inline bool operator!=(const Type& type) const
    {
        return !(*this == type);
    }

    inline bool isPrimitive() const
    {
        return tag <= TypeTag::VOID;
    }
    friend std::ostream& operator<<(std::ostream& os, const Type& val)
    {
        switch (val.tag)
        {
        case TypeTag::BOOL:
            os << "bool";
            break;
        case TypeTag::INTEGER:
            os << "int";
            break;
        case TypeTag::FLOAT:
            os << "float";
            break;
        case TypeTag::DOUBLE:
            os << "double";
            break;
        case TypeTag::CHAR:
            os << "char";
            break;
        case TypeTag::VOID:
            os << "void";
            break;
        case TypeTag::STRING:
            os << "string";
            break;
        case TypeTag::ARRAY:
            os << "[]";
            break;
        case TypeTag::POINTER:
            os << "*";
            break;
        case TypeTag::FUNCTION:
            os << "func";
            break;
        case TypeTag::NATIVE:
            os << "funn";
            break;
        case TypeTag::NULL_TYPE:
            os << "nl";
            break;
        case TypeTag::ERROR:
            os << "err";
            break;
        default:
            std::cout << "[ERROR] Unknown type: " << (int)val.tag << "\n";
            break;
        }

        if (val.intrinsicType)
            os << val.intrinsicType;

        return os;
    }
    size_t getSize() const
    {
        switch (tag)
        {
        case TypeTag::BOOL:
        case TypeTag::INTEGER:
        case TypeTag::FLOAT:
        case TypeTag::DOUBLE:
        case TypeTag::CHAR:
        case TypeTag::FUNCTION:
        case TypeTag::POINTER:
        case TypeTag::STRING:
        case TypeTag::NATIVE:
            return 1;
        case TypeTag::VOID:
            return 0;

        case TypeTag::ARRAY:
            return 1;

        default:
            std::cout << "[ERROR] Unknown type: " << (int)tag << "\n";
            return 0;
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
    Data(*valNative)(int, Data*);
};

typedef Data (*NativeFn)(int, Data*);

class Value
{
public:
    Type type;
    Data data;

    Value()
        : type(TypeTag::ERROR), data({0})
    {
    }

    Value(Type type)
        : type(type), data({0})
    {
    }

    Value(bool value)
        : type(TypeTag::BOOL)
    {
        data.valBool = value;
    }

    Value(char value)
        : type(TypeTag::CHAR)
    {
        data.valChar = value;
    }

    Value(int32_t value)
        : type(TypeTag::INTEGER)
    {
        data.valInt = value;
    }

    Value(float value)
        : type(TypeTag::FLOAT)
    {
        data.valFloat = value;
    }

    Value(double value)
        : type(TypeTag::DOUBLE)
    {
        data.valDouble = value;
    }

    Value(const std::string& str)
        : type(TypeTag::STRING)
    {
        data.valString = new char[str.size() + 1];
        str.copy(data.valString, str.size(), 0);
    }

    Value(char* str)
        : type(TypeTag::STRING)
    {
        data.valString = str;
    }

    virtual ~Value() {}

    friend std::ostream& operator<<(std::ostream& os, const Value& val)
    {
        switch (val.type.tag)
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

        switch (this->type.tag)
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
    std::vector<std::pair<Token, Type>> params;
    IRChunk* irChunk;

    FuncValue(Type returnType, std::vector<std::pair<Token, Type>> params)
        : Value(Type(TypeTag::FUNCTION, std::make_shared<Type>(returnType))), params(params), irChunk(nullptr)
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
    std::vector<Type> params;
    bool is_variadic;

    NativeFunc(NativeFn func, Type returnType, std::vector<Type> params, bool is_variadic = false)
        : Value(Type(TypeTag::NATIVE, std::make_shared<Type>(returnType))), params(params), is_variadic(is_variadic)
    {
        data.valNative = func;
    }

    virtual ~NativeFunc() {}
};
