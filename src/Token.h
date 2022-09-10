//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <string_view>
#include <vector>
#include "Type.h"
#include "Value.h"

enum class TokenType
{
	// One char
	OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET, CLOSE_BRACKET, OPEN_PAREN, CLOSE_PAREN,
	LESS, GREAT, DOT, COMMA, SEMI_COLON, COLON, STAR, PLUS, MINUS, SLASH, MODULUS,
	BANG, EQUAL, BIT_AND, BIT_OR, BIT_XOR, TILDE,

	// Two char
	DOUBLE_COLON, LESS_EQUAL, GREAT_EQUAL, ARROW, BANG_EQUAL, EQUAL_EQUAL, AND, OR,
	PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, BIT_AND_EQUAL, BIT_OR_EQUAL, BIT_XOR_EQUAL, BITSHIFT_LEFT,
	BITSHIFT_RIGHT, PLUS_PLUS, MINUS_MINUS,

	// Three char
	BITSHIFT_LEFT_EQUAL, BITSHIFT_RIGHT_EQUAL,

	// Literals
	STRING_LITERAL, IDENTIFIER, INTEGER_LITERAL, DOUBLE_LITERAL, FLOAT_LITERAL, CHAR_LITERAL,

	// Keywords
	USING, NAMESPACE,
	CLASS, VAR,
	IF, ELSE,
	INT, FLOAT, DOUBLE, CHAR, STRING, BOOL, VOID,
	WHILE, FOR,
	TRUE, FALSE, NULL_TOKEN,
	RETURN,

	// Util
	ERROR, EOF_TOKEN
};


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