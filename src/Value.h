#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

class IRChunk;

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
            return sizeof(bool);
        case TypeTag::INTEGER:
            return sizeof(int32_t);
        case TypeTag::FLOAT:
            return sizeof(float);
        case TypeTag::DOUBLE:
            return sizeof(double);
        case TypeTag::CHAR:
            return sizeof(char);
        case TypeTag::VOID:
            return 0;

        case TypeTag::STRING:
        case TypeTag::ARRAY:
        case TypeTag::POINTER:
        case TypeTag::FUNCTION:
        case TypeTag::NATIVE:
            return sizeof(void*);

        default:
            std::cout << "[ERROR] Unknown type: " << (int)tag << "\n";
            return 0;
        }
    }
};

class Value
{
public:
    Type type;
    union Data
    {
        bool valBool;
        char valChar;
        int32_t valInt;
        float valFloat;
        double valDouble;
        char* valString;
    } data;

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

    uint8_t* cloneData()
    {
        uint8_t* clone = new uint8_t[type.getSize()];
        switch (type.tag)
        {
        case TypeTag::INTEGER:
            *(int32_t*)clone = data.valInt;
            break;
        case TypeTag::FLOAT:
            *(float*)clone = data.valFloat;
            break;
        case TypeTag::DOUBLE:
            *(double*)clone = data.valDouble;
            break;
        case TypeTag::BOOL:
            *(bool*)clone = data.valBool;
            break;
        case TypeTag::CHAR:
            *(char*)clone = data.valChar;
            break;
        case TypeTag::STRING:
            *(char**)clone = data.valString;
        default:
            std::cout << "Unknown type cloning";
            break;
        }
        return clone;
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
    std::vector<Type> params;
    IRChunk* irChunk;

    FuncValue(IRChunk* chunk, Type returnType, std::vector<Type> params)
        : Value(Type(TypeTag::FUNCTION, std::make_shared<Type>(returnType))), params(params), irChunk(chunk)
    {
    }

    FuncValue(IRChunk* chunk, std::shared_ptr<Type> returnType, std::vector<Type> params)
        : Value(Type(TypeTag::FUNCTION, returnType)), params(params), irChunk(chunk)
    {
    }

    virtual ~FuncValue() {}

    friend std::ostream& operator<<(std::ostream& os, const FuncValue& val)
    {
        os << val.irChunk;
        return os;
    }
};