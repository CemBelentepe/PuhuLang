//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <string>
#include <vector>
#include "Token.h"

class Scanner
{
public:
	explicit Scanner(std::string  source);
	~Scanner();

	std::vector<Token> scan();

private:
	Token scanToken();
	[[nodiscard]] Token makeToken(TokenType type) const;

	Token scanString();
	Token scanChar();
	Token scanIdentifier();
	Token scanNumber();

	void skipWhitespace();

	char advance();
	bool match(char c);
	[[nodiscard]] char peek() const;
	[[nodiscard]] char peekNext() const;
	[[nodiscard]] bool isAtEnd() const;
	[[nodiscard]] std::string_view currentLexeme() const;

	[[nodiscard]] static bool isAlpha(char c) ;
	[[nodiscard]] static bool isNumber(char c) ;

private:
	std::string source;
	unsigned long startPos;
	unsigned long currentPos;
	unsigned long currentLine;
	unsigned long currentCol;
};
