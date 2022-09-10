//
// Created by cembelentepe on 08/09/22.
//

#include <sstream>
#include "Token.h"

Token::Token()
	: type(TokenType::ERROR), lexeme(), line(0), col(0), valInit(false)
{
}

Token::Token(TokenType type, unsigned long line, unsigned long col, std::string_view lexeme)
	: type(type), lexeme(lexeme), line(line), col(col), valInit(false)
{
}

std::string Token::showInfo() const
{
	std::stringstream ss;
	ss << "\"" << lexeme << "\" [" << line << ":" << col << ", type: " << (int)type << "]";
	return ss.str();
}

std::shared_ptr<Type> Token::getType() const
{
	switch (type)
	{
	case TokenType::INTEGER_LITERAL:
		return TypeFactory::getPrimitive(PrimitiveTag::INT);
	case TokenType::FLOAT_LITERAL:
		return TypeFactory::getPrimitive(PrimitiveTag::FLOAT);
	case TokenType::DOUBLE_LITERAL:
		return TypeFactory::getPrimitive(PrimitiveTag::DOUBLE);
	case TokenType::CHAR_LITERAL:
		return TypeFactory::getPrimitive(PrimitiveTag::CHAR);
	case TokenType::TRUE:
	case TokenType::FALSE:
		return TypeFactory::getPrimitive(PrimitiveTag::BOOL);
	case TokenType::STRING_LITERAL:
		return TypeFactory::getString();
	default:
		return TypeFactory::getNull();
	}
}

Value Token::getValue()
{
	if (!valInit)
	{
		switch (type)
		{
		case TokenType::INTEGER_LITERAL:
			val = Value(readAsInt(), getType());
			break;
		case TokenType::FLOAT_LITERAL:
			val = Value(readAsFloat(), getType());
			break;
		case TokenType::DOUBLE_LITERAL:
			val = Value(readAsDouble(), getType());
			break;
		case TokenType::CHAR_LITERAL:
			val = Value(readAsChar(), getType());
			break;
		case TokenType::TRUE:
		case TokenType::FALSE:
			val = Value(readAsBool(), getType());
			break;
		case TokenType::STRING_LITERAL:
			val = Value(readAsString(), getType());
			break;
		default:
			static_assert(true, "Invalid token as a literal");
		}
	}

	return val;
}

bool Token::readAsBool() const
{
	if (type == TokenType::TRUE)
		return true;
	if (type == TokenType::FALSE)
		return false;
	throw std::runtime_error("Invalid token for 'bool' literal");
}

char Token::readAsChar() const
{
	std::string_view chr = lexeme.substr(1, lexeme.size() - 2);
	if (chr.size() == 1)
	{
		return chr[0];
	}
	else if (chr.size() == 2 && chr[0] == '\\')
	{
		return getEscapeCharacter(chr[1]);
	}
	throw std::runtime_error("Invalid token for 'char' literal");
}

int Token::readAsInt() const
{
	std::stringstream ss;
	ss << lexeme;
	int res;
	ss >> res;
	return res;
}

float Token::readAsFloat() const
{
	if (lexeme.back() != 'f')
		throw std::runtime_error("Invalid token for 'float' literal");

	std::stringstream ss;
	ss << lexeme.substr(0, lexeme.size() - 1);
	float res;
	ss >> res;
	return res;
}

double Token::readAsDouble() const
{
	std::stringstream ss;
	ss << lexeme.substr(0, lexeme.size() - 1);
	double res;
	ss >> res;
	return res;
}

std::string Token::readAsString() const
{
	std::stringstream ss;

	size_t i = 0;
	while (i < lexeme.size())
	{
		if (lexeme[i] == '\\')
		{
			ss << getEscapeCharacter(lexeme[i + 1]);
			i++;
		}
		else
		{
			ss << lexeme[i];
		}
		i++;
	}

	return ss.str();
}
char Token::getEscapeCharacter(char c)
{
	switch (c)
	{
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	case '\\': return '\\';
	case '\'': return '\'';
	case '"': return '"';
	case '?': return '\?';
	default:
		throw std::runtime_error("Invalid character for escape sequence.");
	}
}


