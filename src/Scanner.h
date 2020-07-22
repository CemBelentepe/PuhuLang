#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class TokenType
{
	// One char
	OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET, CLOSE_BRACKET, OPEN_PAREN, CLOSE_PAREN,
	LESS, GREAT, DOT, COMMA, SEMI_COLON, COLON, STAR, PLUS, MINUS, SLASH, MODULUS,
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
	USING, NAMESPACE, CLASS,
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
	const int line;
	const char* start;
	const int length;

	Token(TokenType type, int line, const char* start, int length)
		:type(type), line(line), start(start), length(length)
	{}

	friend std::ostream& operator<<(std::ostream& os, const Token& token);
	int getInteger();
	double getDouble();
	float getFloat();
	std::string getString();
	char getChar();
};

class Scanner
{
public:
	Scanner(std::string& source);
	std::vector<Token> scanTokens(); // scans all of the tokens in the project

private:
	std::string& source;
	int startPosition; // current position of the scanner
	int currentPosition; // current position of the scanner
	int line; // current line of the scanner

	Token scanToken(); // scans the next token on the file
	Token makeToken(TokenType type); // creates a new token with the type

	std::string formatString(const char* str, size_t size);
	Token stringLiteral(); // scans to a "
	Token charLiteral(); // scans to a '
	Token identifierLiteral(char start); // scans the next identifier
	Token numberLiteral(); // scans the next number
	Token errorToken(const char* msg);

	void skipWhitespace(); // advances while in a whitespace

	char advance(); // Consumes the next char and returns the consumed char
	bool match(char c); // returns whether if the next char is c if so consumes
	char peekPrev(); // returns the previous char
	char peek(); // returns the current char
	char peekNext(); // return the next char
	bool isAtEnd(); // is at the end of the file

	inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
	inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
};
