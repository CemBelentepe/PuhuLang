#pragma once
#include "Log.hpp"

#include <string>
#include <vector>
#include <string_view>

enum class TokenType
{
	// One char
	OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET, CLOSE_BRACKET, OPEN_PAREN, CLOSE_PAREN,
	LESS, GREAT, DOT, COMMA, SEMI_COLON, DOUBLE_COLON, COLON, STAR, PLUS, MINUS, SLASH, MODULUS,
	BANG, EQUAL, BIT_AND, BIT_OR, BIT_XOR, TILDE,

	// Two char
	LESS_EQUAL, GREAT_EQUAL, ARROW, BANG_EQUAL, EQUAL_EQUAL, AND, OR,
	PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, BIT_AND_EQUAL, BIT_OR_EQUAL, BIT_XOR_EQUAL, BITSHIFT_LEFT, BITSHIFT_RIGHT,
	PLUS_PLUS, MINUS_MINUS,

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
	HEAP, REF, TAKE,

	// Util
	ERROR, EOF_TOKEN
};

class Token
{
public:
	static std::vector<std::string_view> lines;
    TokenType type;
    std::string_view lexeme;
    int line;
    int col;

    Token();
    Token(TokenType type, int line, int col, std::string_view lexeme);

	int getInteger() const;
	double getDouble() const;
	float getFloat() const;
	std::string getString() const; // TODO: add formatting to here too
	char getChar() const; // TODO: add formatting and size checking
	void showInLine(Logger::LogColor color = Logger::RED) const;
	std::string toStringInLine(Logger::LogColor color = Logger::RED) const;

	friend std::ostream& operator<<(std::ostream& os, const Token& token);
};
