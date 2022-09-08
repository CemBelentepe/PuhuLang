//
// Created by cembelentepe on 08/09/22.
//

#include <algorithm>
#include "Scanner.h"

Scanner::Scanner(const std::string& source)
		: source(source), startPos(0), currentPos(0), currentLine(1), currentCol(0)
{
}

Scanner::~Scanner()
{
}

std::vector<Token> Scanner::scan()
{
	std::vector<Token> tokens;

	do
	{
		tokens.push_back(scanToken());
	} while (tokens.back().type != TokenType::EOF_TOKEN);

	return tokens;
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
			return makeToken(TokenType::ERROR);
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

	// TODO string problems
	// if (isAtEnd()) throw ScannerError
	// if (!match('"')) throw ScannerError

	advance(); // consume '"'

	return makeToken(TokenType::STRING_LITERAL);
}

Token Scanner::scanChar()
{
	char prev = '\'';
	while ((peek() != '\'' || prev == '\\') && !isAtEnd())
	{
		prev = advance();
	}

	// TODO char problems
	// if (isAtEnd()) throw ScannerError
	// if (!match('"')) throw ScannerError
	// check char literal size here !
	advance(); // consume '\''

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
				advance();
				advance();
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
	while (isAlpha(peek()) || isNumber(peek()))
		advance();

	std::string_view lexeme = currentLexeme();

	if (lexeme == "using") return makeToken(TokenType::USING);
	if (lexeme == "namespace") return makeToken(TokenType::NAMESPACE);
	if (lexeme == "class") return makeToken(TokenType::CLASS);
	if (lexeme == "var") return makeToken(TokenType::VAR);
	if (lexeme == "if") return makeToken(TokenType::IF);
	if (lexeme == "else") return makeToken(TokenType::ELSE);
	if (lexeme == "int") return makeToken(TokenType::INT);
	if (lexeme == "float") return makeToken(TokenType::FLOAT);
	if (lexeme == "double") return makeToken(TokenType::DOUBLE);
	if (lexeme == "char") return makeToken(TokenType::CHAR);
	if (lexeme == "string") return makeToken(TokenType::STRING);
	if (lexeme == "bool") return makeToken(TokenType::BOOL);
	if (lexeme == "void") return makeToken(TokenType::VOID);
	if (lexeme == "while") return makeToken(TokenType::WHILE);
	if (lexeme == "for") return makeToken(TokenType::FOR);
	if (lexeme == "true") return makeToken(TokenType::TRUE);
	if (lexeme == "false") return makeToken(TokenType::FALSE);
	if (lexeme == "null") return makeToken(TokenType::NULL_TOKEN);
	if (lexeme == "return") return makeToken(TokenType::RETURN);

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

bool Scanner::match(std::vector<char> c)
{
	if (std::find(c.begin(), c.end(), source[currentPos]) != c.end())
	{
		advance();
		return true;
	}
	return false;
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

bool Scanner::isAlpha(char c) const
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::isNumber(char c) const
{
	return (c >= '0' && c <= '9');
}

std::string_view Scanner::currentLexeme() const
{
	std::string_view lexeme(source);
	lexeme = lexeme.substr(startPos, currentPos - startPos);
	return lexeme;
}




