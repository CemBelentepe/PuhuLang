//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "Token.h"

class Type;
using TypePtr = std::shared_ptr<Type>;

class Type
{
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
	TypePtr intrinsicType;
	bool isConst;

	Type() = delete;
	explicit Type(Tag tag, TypePtr intrinsicType, bool isConst = false)
			: tag(tag), intrinsicType(std::move(intrinsicType)), isConst(isConst)
	{
	}

	virtual ~Type() = default;

	[[nodiscard]] virtual bool isSame(const std::shared_ptr<Type>& other) const = 0;
	virtual std::string toString() = 0;
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

enum class PrimitiveTag
{
	VOID,
	BOOL,
	CHAR,
	INT,
	FLOAT,
	DOUBLE
};

class TypePrimitive : public Type
{
public:
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
	explicit TypePointer(TypePtr intrinsic)
			: Type(Tag::POINTER, std::move(intrinsic))
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;
	std::string toString() override;
};

class TypeArray: public Type
{
public:
	explicit TypeArray(TypePtr intrinsic)
			: Type(Tag::POINTER, std::move(intrinsic))
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

// TODO Implement a cache for the type factory
class TypeFactory
{
public:
	static TypePtr getNull();
	static TypePtr getPrimitive(PrimitiveTag tag);
	static TypePtr getString();
	static TypePtr getArray(const std::shared_ptr<Type>& intrinsicType);
	static TypePtr getPointer(const std::shared_ptr<Type>& intrinsicType);
	static TypePtr getFunction(const TypePtr& ret_type, const std::vector<TypePtr>& param_types = {});

	static TypePtr fromToken(TokenType token);
};
