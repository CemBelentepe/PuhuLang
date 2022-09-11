//
// Created by cembelentepe on 08/09/22.
//

#include "Scanner.h"

#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <sstream>

Scanner::Scanner(std::string source)
		: source(std::move(source)), startPos(0), currentPos(0), currentLine(1), currentCol(0), failed(false)
{
}

Scanner::~Scanner() = default;

std::vector<Token> Scanner::scan()
{
	std::vector<Token> tokens;

	do
	{
		try
		{
			tokens.push_back(scanToken());
		}
		catch (std::runtime_error& e)
		{
			std::cerr << e.what() << std::endl;
			failed = true;
		}
	} while (tokens.back().type != TokenType::EOF_TOKEN);

	return tokens;
}

bool Scanner::fail() const
{
	return failed;
}

Token Scanner::scanToken()
{
	skipWhitespace();

	startPos = currentPos;
	if (isAtEnd())
		return makeToken(TokenType::EOF_TOKEN);

	char c = advance();
	switch (c)
	{
	case '{':
		return makeToken(TokenType::OPEN_BRACE);
	case '}':
		return makeToken(TokenType::CLOSE_BRACE);
	case '[':
		return makeToken(TokenType::OPEN_BRACKET);
	case ']':
		return makeToken(TokenType::CLOSE_BRACKET);
	case '(':
		return makeToken(TokenType::OPEN_PAREN);
	case ')':
		return makeToken(TokenType::CLOSE_PAREN);
	case '<':
		if (match('='))
			return makeToken(TokenType::LESS_EQUAL);
		if (match('<'))
		{
			if (match('='))
				return makeToken(TokenType::BITSHIFT_LEFT_EQUAL);
			return makeToken(TokenType::BITSHIFT_LEFT);
		}
		return makeToken(TokenType::LESS);
	case '>':
		if (match('='))
			return makeToken(TokenType::GREAT_EQUAL);
		if (match('>'))
		{
			if (match('='))
				return makeToken(TokenType::BITSHIFT_RIGHT_EQUAL);
			return makeToken(TokenType::BITSHIFT_RIGHT);
		}
		return makeToken(TokenType::GREAT);
	case '.':
		return makeToken(TokenType::DOT);
	case ',':
		return makeToken(TokenType::COMMA);
	case ';':
		return makeToken(TokenType::SEMI_COLON);
	case ':':
		if (match(':'))
			return makeToken(TokenType::DOUBLE_COLON);
		return makeToken(TokenType::COLON);
	case '*':
		if (match('='))
			return makeToken(TokenType::STAR_EQUAL);
		return makeToken(TokenType::STAR);
	case '/':
		if (match('='))
			return makeToken(TokenType::SLASH_EQUAL);
		return makeToken(TokenType::SLASH);
	case '+':
		if (match('='))
			return makeToken(TokenType::PLUS_EQUAL);
		if (match('+'))
			return makeToken(TokenType::PLUS_PLUS);
		return makeToken(TokenType::PLUS);
	case '-':
		if (match('>'))
			return makeToken(TokenType::ARROW);
		if (match('='))
			return makeToken(TokenType::MINUS_EQUAL);
		if (match('-'))
			return makeToken(TokenType::MINUS_MINUS);
		return makeToken(TokenType::MINUS);
	case '%':
		return makeToken(TokenType::MODULUS);
	case '!':
		if (match('='))
			return makeToken(TokenType::BANG_EQUAL);
		return makeToken(TokenType::BANG);
	case '=':
		if (match('='))
			return makeToken(TokenType::EQUAL_EQUAL);
		return makeToken(TokenType::EQUAL);
	case '&':
		if (match('&'))
			return makeToken(TokenType::AND);
		if (match('='))
			return makeToken(TokenType::BIT_AND_EQUAL);
		return makeToken(TokenType::BIT_AND);
	case '|':
		if (match('|'))
			return makeToken(TokenType::OR);
		if (match('='))
			return makeToken(TokenType::BIT_OR_EQUAL);
		return makeToken(TokenType::BIT_OR);
	case '^':
		if (match('='))
			return makeToken(TokenType::BIT_XOR_EQUAL);
		return makeToken(TokenType::BIT_XOR);
	case '~':
		return makeToken(TokenType::TILDE);
	case '"':
		return scanString();
	case '\'':
		return scanChar();
	default:
	{
		if (isNumber(c))
			return scanNumber();
		else if (isAlpha(c))
			return scanIdentifier();
		else
		{
			std::stringstream ssErr;
			ssErr << "[ERROR " << currentLine << ":" << currentCol << "] Unexpected character '" << c << "'";
			throw std::runtime_error(ssErr.str());
		}
	}
	}
}

Token Scanner::makeToken(TokenType type) const
{
	std::string_view lexeme = currentLexeme();
	unsigned long len = lexeme.length();
	return { type, currentLine, currentCol - len + 1, lexeme };
}

Token Scanner::scanString()
{
	while (peek() != '"' && !isAtEnd())
	{
		char c = advance();
		if (c == '\\')
			advance();
	}

	consume('"', "String literal is not terminated.");

	return makeToken(TokenType::STRING_LITERAL);
}

Token Scanner::scanChar()
{
	char prev = '\'';
	while (peek() != '\'' && !isAtEnd())
	{
		prev = advance();
		if (prev == '\\')
			prev = advance();
	}

	consume('\'', "Character literal is not terminated.");

	return makeToken(TokenType::CHAR_LITERAL);
}

Token Scanner::scanNumber()
{
	TokenType type = TokenType::INTEGER_LITERAL;
	while (isNumber(peek()))
		advance();
	if (match('.'))
	{
		type = TokenType::DOUBLE_LITERAL;
		advance();
		while (isNumber(peek()))
			advance();
		if (match('f'))
			type = TokenType::FLOAT_LITERAL;
	}

	return makeToken(type);
}

void Scanner::skipWhitespace()
{
	while (true)
	{
		char c = peek();
		switch (c)
		{
		case ' ':
		case '\t':
		case '\r':
			advance();
			break;
		case '\n':
			advance();
			currentCol = 0;
			currentLine++;
			break;
		case '/':
			if (peekNext() == '/')
			{
				advance();
				advance();
				while (peek() != '\n' && !isAtEnd())
				{
					advance();
				}
				skipWhitespace();
			}
			else if (peekNext() == '*')
			{
				advance();
				advance();
				while (!(peek() == '*' && peekNext() == '/') && !isAtEnd())
				{
					char x = advance();
					if (x == '\n')
					{
						currentCol = 0;
						currentLine++;
					}
				}
				consume('*', "Unterminated comment.");
				consume('/', "Unterminated comment.");
				skipWhitespace();
			}
			return;
		default:
			return;
		}
	}
}

Token Scanner::scanIdentifier()
{
	while (isAlpha(peek()) || isNumber(peek()) || peek() == '_')
		advance();

	std::string_view lexeme = currentLexeme();

	static const std::unordered_map<std::string, TokenType> lut = {
			{ "using",     TokenType::USING },
			{ "namespace", TokenType::NAMESPACE },
			{ "class",     TokenType::CLASS },
			{ "var",       TokenType::VAR },
			{ "const",     TokenType::CONST },
			{ "if",        TokenType::IF },
			{ "else",      TokenType::ELSE },
			{ "int",       TokenType::INT },
			{ "float",     TokenType::FLOAT },
			{ "double",    TokenType::DOUBLE },
			{ "char",      TokenType::CHAR },
			{ "string",    TokenType::STRING },
			{ "bool",      TokenType::BOOL },
			{ "void",      TokenType::VOID },
			{ "while",     TokenType::WHILE },
			{ "for",       TokenType::FOR },
			{ "true",      TokenType::TRUE },
			{ "false",     TokenType::FALSE },
			{ "null",      TokenType::NULL_TOKEN },
			{ "return",    TokenType::RETURN },
	};

	auto it = lut.find(std::string(lexeme));
	if (it != lut.end())
		return makeToken(it->second);

	return makeToken(TokenType::IDENTIFIER);
}

char Scanner::advance()
{
	char c = source[currentPos];
	currentPos++;
	currentCol++;
	return c;
}

bool Scanner::match(char c)
{
	if (source[currentPos] == c)
	{
		advance();
		return true;
	}
	return false;
}

void Scanner::consume(char c, const std::string& msg)
{
	if (!match(c))
	{
		advance();
		std::stringstream ssErr;
		ssErr << "[ERROR " << currentLine << ":" << currentCol << "] " << msg << "\n";

		throw std::runtime_error(ssErr.str());
	}
}

char Scanner::peek() const
{
	return source[currentPos];
}

char Scanner::peekNext() const
{
	return source[currentPos + 1];
}

bool Scanner::isAtEnd() const
{
	return currentPos >= source.length();
}

bool Scanner::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::isNumber(char c)
{
	return (c >= '0' && c <= '9');
}

std::string_view Scanner::currentLexeme() const
{
	std::string_view lexeme(source);
	lexeme = lexeme.substr(startPos, currentPos - startPos);
	return lexeme;
}

