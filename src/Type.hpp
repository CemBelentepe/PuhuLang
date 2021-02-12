#pragma once
#include <iostream>
#include <memory>
#include <string>

class Type
{
public:
    enum class Tag
    {
        PRIMITIVE, POINTER, FUNTION, STRING, ARRAY, USER_DEF, ERROR = -1
    }; 
    Tag tag;
    std::shared_ptr<Type> instrinsicType;

    Type() = delete;
    explicit Type(Tag tag, std::shared_ptr<Type> instrinsicType)
        : tag(tag), instrinsicType(instrinsicType)
    {}

    virtual ~Type() = default;

    virtual std::string toString() = 0;

    static std::shared_ptr<Type> getNullType(); // TODO make it so that it holds all the created types
};

class TypeError : public Type
{
public:
    explicit TypeError()
        : Type(Tag::ERROR, nullptr)
    {}

    std::string toString() override;
};

class TypePrimitive : public Type
{
public:
    enum class PrimitiveTag { VOID, BOOL, CHAR, INT, FLOAT, DOUBLE };
    PrimitiveTag special_tag;

    explicit TypePrimitive(PrimitiveTag special_tag)
        : Type(Tag::PRIMITIVE, nullptr), special_tag(special_tag)
    {}

    std::string toString() override;
};

class TypeString : public Type
{
public:
    explicit TypeString()
        : Type(Tag::STRING, nullptr)
    {}

    std::string toString() override;
};
