//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include <utility>
#include <vector>

// TODO Implement a TypeFactory class, pls :(

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
	explicit Type(Tag tag, TypePtr intrinsicType)
			: tag(tag), instrinsicType(std::move(intrinsicType))
	{
	}

	virtual ~Type() = default;

	[[nodiscard]] virtual bool isSame(const std::shared_ptr<Type>& other) const = 0;
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

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
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

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
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

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
	std::string toString() override;
};

class TypeString : public Type
{
public:
	explicit TypeString()
			: Type(Tag::STRING, nullptr)
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
	std::string toString() override;
};

class TypeFunction : public Type
{
public:
	std::vector<TypePtr> param_types;

	explicit TypeFunction(TypePtr ret_type, std::vector<TypePtr> param_types = {})
			: Type(Type::Tag::FUNCTION, (std::move(ret_type))), param_types(std::move(param_types))
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
	std::string toString() override;
};