//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <utility>
#include <vector>
#include "TokenType.h"
#include "Token.h"
#include "Common.h"

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
	std::string toString();

protected:
	virtual std::string toStringHook() = 0;
};

class TypeError : public Type
{
public:
	explicit TypeError()
			: Type(Tag::ERROR, nullptr)
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;

protected:
	std::string toStringHook() override;
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

protected:
	std::string toStringHook() override;
};

class TypePointer : public Type
{
public:
	explicit TypePointer(TypePtr intrinsic)
			: Type(Tag::POINTER, std::move(intrinsic))
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;

protected:
	std::string toStringHook() override;
};

class TypeArray: public Type
{
public:
	explicit TypeArray(TypePtr intrinsic)
			: Type(Tag::ARRAY, std::move(intrinsic))
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;

protected:
	std::string toStringHook() override;
};

class TypeString : public Type
{
public:
	explicit TypeString()
			: Type(Tag::STRING, nullptr)
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;

protected:
	std::string toStringHook() override;
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

protected:
	std::string toStringHook() override;
};

class TypeUserDefined : public Type
{
public:
	Token name;

	explicit TypeUserDefined(Token name)
		: Type(Tag::USER_DEF, nullptr), name(name)
	{
	}

	[[nodiscard]] bool isSame(const std::shared_ptr<Type>& other) const override;

protected:
	std::string toStringHook() override;
};

// TODO Implement a cache for the type factory
class TypeFactory
{
public:
	static std::shared_ptr<TypeError> 		getNull();
	static std::shared_ptr<TypePrimitive> 	getPrimitive(PrimitiveTag tag);
	static std::shared_ptr<TypeString> 		getString();
	static std::shared_ptr<TypeArray> 		getArray(const std::shared_ptr<Type>& intrinsicType);
	static std::shared_ptr<TypePointer> 	getPointer(const std::shared_ptr<Type>& intrinsicType);
	static std::shared_ptr<TypeFunction> 	getFunction(const TypePtr& ret_type, const std::vector<TypePtr>& param_types = {});
	static std::shared_ptr<TypeUserDefined> getUserDefined(Token name);

	static TypePtr fromToken(Token token);
};
