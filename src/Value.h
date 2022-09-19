//
// Created by cembelentepe on 09/09/22.
//

#pragma once

#include <string>
#include <variant>
#include <vector>
#include "Common.h"

class Callable;
class Token;

class Value
{
public:
	using Data = std::variant<bool, char, int, float, double, std::string, std::shared_ptr<Callable>, std::shared_ptr<Value>>;

	explicit Value();
	explicit Value(Data data, TypePtr type);

	[[nodiscard]] Data getData() const;
	[[nodiscard]] TypePtr getType() const;

	[[nodiscard]] std::string getInfo() const;

	template<typename T>
	[[nodiscard]] T getDataTyped() const
	{
		return std::get<T>(data);
	}

	static Value getVoid();

private:
	Data data;
	TypePtr type;
};
