//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <string_view>
#include <vector>
#include "Value.h"
#include "TokenType.h"

class Type;

class Token
{
public:
	TokenType type;
	std::string_view lexeme;
	unsigned long line;
	unsigned long col;
	Value val;

	Token();
	Token(TokenType type, unsigned long line, unsigned long col, std::string_view lexeme);

	[[nodiscard]] std::string showInfo() const;
	[[nodiscard]] std::shared_ptr<Type> getType() const;
	Value getValue();

	[[nodiscard]] std::string getLexeme() const;

private:
	[[nodiscard]] bool readAsBool() const;
	[[nodiscard]] char readAsChar() const;
	[[nodiscard]] int readAsInt() const;
	[[nodiscard]] float readAsFloat() const;
	[[nodiscard]] double readAsDouble() const;
	[[nodiscard]] std::string readAsString() const;

	[[nodiscard]] static char getEscapeCharacter(char c);

private:
	bool valInit;
};
