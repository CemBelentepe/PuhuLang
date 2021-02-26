#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Type
{
protected:
    using TypePtr = std::shared_ptr<Type>;

public:
    enum class Tag
    {
        PRIMITIVE,
        STRING,
        POINTER,
        FUNCTION,
        ARRAY,
        USER_DEF,
        ERROR = -1
    };
    Tag tag;
    TypePtr instrinsicType;

    Type() = delete;
    explicit Type(Tag tag, TypePtr instrinsicType)
        : tag(tag), instrinsicType(instrinsicType)
    {
    }

    virtual ~Type() = default;

    virtual bool isSame(const std::shared_ptr<Type>& other) const = 0;
    virtual std::string toString() = 0;

    static TypePtr getNullType(); // TODO make it so that it holds all the created types
};

class TypeError : public Type
{
public:
    explicit TypeError()
        : Type(Tag::ERROR, nullptr)
    {
    }

    bool isSame(const std::shared_ptr<Type>& other) const override;
    std::string toString() override;
};

class TypePrimitive : public Type
{
public:
    enum class PrimitiveTag
    {
        VOID,
        BOOL,
        CHAR,
        INT,
        FLOAT,
        DOUBLE
    };
    PrimitiveTag special_tag;

    explicit TypePrimitive(PrimitiveTag special_tag)
        : Type(Tag::PRIMITIVE, nullptr), special_tag(special_tag)
    {
    }

    bool isSame(const std::shared_ptr<Type>& other) const override;
    std::string toString() override;
};

class TypePointer : public Type
{
public:
    bool isOwner;
    explicit TypePointer(TypePtr intrinsic, bool isOwner)
        : Type(Tag::POINTER, std::move(intrinsic)), isOwner(isOwner)
    {
    }
    
    bool isSame(const std::shared_ptr<Type>& other) const override;
    std::string toString() override;
};

class TypeString : public Type
{
public:
    explicit TypeString()
        : Type(Tag::STRING, nullptr)
    {
    }

    bool isSame(const std::shared_ptr<Type>& other) const override;
    std::string toString() override;
};

class TypeFunction : public Type
{
public:
    std::vector<TypePtr> param_types;

    explicit TypeFunction(TypePtr ret_type, const std::vector<TypePtr>& param_types = {})
        : Type(Type::Tag::FUNCTION, ret_type), param_types(param_types)
    {
    }

    bool isSame(const std::shared_ptr<Type>& other) const override;
    std::string toString() override;
};
