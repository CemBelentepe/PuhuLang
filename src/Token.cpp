//
// Created by cembelentepe on 08/09/22.
//

#include <sstream>
#include "Token.h"

Token::Token()
		: type(TokenType::ERROR), lexeme(), line(0), col(0)
{
}

Token::Token(TokenType type, unsigned long line, unsigned long col, std::string_view lexeme)
		: type(type), lexeme(lexeme), line(line), col(col)
{
}

std::string Token::showInfo() const
{
	std::stringstream ss;
	ss << "\"" << lexeme << "\" [" << line << ":" << col << ", type: " << (int)type << "]";
	return ss.str();
}

std::shared_ptr<Type> Token::getType()
{
	switch (type)
	{
	case TokenType::INTEGER_LITERAL:
		return std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::INT);
	case TokenType::FLOAT_LITERAL:
		return std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::FLOAT);
	case TokenType::DOUBLE_LITERAL:
		return std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::DOUBLE);
	case TokenType::CHAR_LITERAL:
		return std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::CHAR);
	case TokenType::STRING_LITERAL:
		return std::make_shared<TypeString>();
	default:
		return Type::getNullType();
	}
}


