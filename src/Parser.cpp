#include "Parser.h"
#include "AST.h"

Parser::Parser(std::vector<Token>& tokens)
	:tokens(tokens), currentToken(0)
{}

Expr* Parser::parse()
{
    this->currentToken = 0;
    Expr* expr = parseExpression();
    return expr;
}

Type Parser::parseTypeName()
{
	Type type;
	TypeTag val = getDataType(peek());
	int i = 0;
	do
	{
		if (i > 0)
			type.intrinsicType = std::make_shared<Type>(type);
		type.tag = val;
		if (type.isPrimitive() && type.intrinsicType != nullptr && type.intrinsicType->isPrimitive())
		{
			error("Invalid type name.");
			break;
		}

		advance();
		i++;
	} while ((val = getDataType(peek())) != TypeTag::ERROR);

	return type;
}

bool Parser::isTypeName(Token& token)
{
	switch (token.type)
	{
	case TokenType::INT:
	case TokenType::FLOAT:
	case TokenType::DOUBLE:
	case TokenType::CHAR:
	case TokenType::STRING:
	case TokenType::BOOL:
	case TokenType::VOID:
		return true;
	default:
		return false;
	}
}

TypeTag Parser::getDataType(Token& token)
{
	switch (token.type)
	{
	case TokenType::INT:
		return TypeTag::INTEGER;
	case TokenType::FLOAT:
		return TypeTag::FLOAT;
	case TokenType::DOUBLE:
		return TypeTag::DOUBLE;
	case TokenType::CHAR:
		return TypeTag::CHAR;
	case TokenType::STRING:
		return TypeTag::STRING;
	case TokenType::BOOL:
		return TypeTag::BOOL;
	case TokenType::VOID:
		return TypeTag::VOID;
	case TokenType::STAR:
		return TypeTag::POINTER;
	case TokenType::OPEN_BRACKET:
		if (peekNext().type == TokenType::CLOSE_BRACKET)
		{
			advance();
			return TypeTag::ARRAY;
		}
		else
			return TypeTag::ERROR;
	default:
		return TypeTag::ERROR;
	}
}

void Parser::consume(TokenType type, const char* message)
{
	if (tokens[currentToken].type == type)
	{
		advance();
	}
	else
	{
		errorAtToken(message);
	}

}

void Parser::consumeNext(TokenType type, const char* message)
{
	if (tokens[currentToken + 1].type == type)
	{
		advance();
	}
	else
	{
		errorAtToken(message);
	}

}

bool Parser::match(TokenType type)
{
	TokenType next = tokens[currentToken].type;
	if (next == type)
	{
		currentToken++;
		return true;
	}

	return false;
}

bool Parser::matchCast()
{
	return tokens[currentToken].type == TokenType::OPEN_PAREN && getCast() != TypeTag::ERROR && tokens[currentToken + 2].type == TokenType::CLOSE_PAREN;
}

Type Parser::getCast()
{
	switch (tokens[currentToken + 1].type)
	{
	case TokenType::INT:
		return TypeTag::INTEGER;
	case TokenType::FLOAT:
		return TypeTag::FLOAT;
	case TokenType::DOUBLE:
		return TypeTag::DOUBLE;
	case TokenType::CHAR:
		return TypeTag::CHAR;
	case TokenType::STRING:
		return TypeTag::STRING;
	case TokenType::BOOL:
		return TypeTag::BOOL;
	case TokenType::VOID:
		return TypeTag::VOID;
	default:
		return TypeTag::ERROR;
	}
}

inline Expr* Parser::typeError(const char* message) const
{
	std::cout << "At " << tokens[currentToken - 1] << message << std::endl;
	return nullptr;
}

inline void Parser::error(const char* message)
{
	std::cout << message << std::endl;
	this->panic();
}

inline void Parser::errorAtToken(const char* message)
{
	std::cout << "[line" << tokens[currentToken].line << "] Error" << message << std::endl;
	this->panic();
}

void Parser::panic()
{
	TokenType type = tokens[currentToken].type;
	while (type != TokenType::EOF_TOKEN)
	{
		if (type == TokenType::SEMI_COLON)
		{
			advance();
			return;
		}

		switch (type)
		{
		case TokenType::USING:
		case TokenType::NAMESPACE:
		case TokenType::CLASS:
		case TokenType::IF:
		case TokenType::INT:
		case TokenType::FLOAT:
		case TokenType::DOUBLE:
		case TokenType::CHAR:
		case TokenType::STRING:
		case TokenType::BOOL:
		case TokenType::VOID:
		case TokenType::WHILE:
		case TokenType::FOR:
		case TokenType::RETURN:
			return;

		case TokenType::IDENTIFIER:
			// TODO: add user defined types
			// if (isTypeName(tokens[currentToken]))
			return;

		default:
			break;
		}

		type = advance().type;
	}
}

Expr* Parser::parseExpression()
{
	return bit_or();
}

Expr* Parser::bit_or()
{
	Expr* left = bit_xor();
	while (match(TokenType::BIT_OR))
	{
		Token op = consumed();
		Expr* right = bit_xor();
		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::bit_xor()
{
	Expr* left = bit_and();
	while (match(TokenType::BIT_XOR))
	{
		Token op = consumed();
		Expr* right = bit_and();
		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::bit_and()
{
	Expr* left = equality();
	while (match(TokenType::BIT_AND))
	{
		Token op = consumed();
		Expr* right = equality();
		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::equality()
{
	Expr* left = comparison();
	while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL))
	{
		Token op = consumed();
		Expr* right = comparison();

		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::comparison()
{
	Expr* left = bitshift();
	while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) || match(TokenType::GREAT) || match(TokenType::GREAT_EQUAL))
	{
		Token op = consumed();
		Expr* right = bitshift();

		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::bitshift()
{
	Expr* left = addition();
	while (match(TokenType::BITSHIFT_LEFT) || match(TokenType::BITSHIFT_RIGHT))
	{
		Token op = consumed();
		Expr* right = addition();
		left = new ExprBinary(left, right, op);
	}
	return left;
}

Expr* Parser::addition()
{
	Expr* left = multiplication();
	while (match(TokenType::PLUS) || match(TokenType::MINUS))
	{
		Token op = consumed();
		Expr* right = multiplication();

		left = new ExprBinary(left, right, op);
	}

	return left;
}

Expr* Parser::multiplication()
{
	Expr* left = prefix();
	while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MODULUS))
	{
		Token op = consumed();
		Expr* right = prefix();

		left = new ExprBinary(left, right, op);
	}

	return left;
}

Expr* Parser::prefix()
{
	if (match(TokenType::MINUS) || match(TokenType::PLUS) || match(TokenType::MINUS_MINUS) || match(TokenType::PLUS_PLUS))
	{
		Token op = consumed();
		Expr* expr = unary();
		return new ExprUnary(expr, op);
	}
	else
		return unary();
}

Expr* Parser::unary()
{
	if (match(TokenType::BANG) || match(TokenType::TILDE))
	{
		Token op = consumed();
		Expr* expr = unary();
		return new ExprUnary(expr, op);
	}
	else if (matchCast())
	{
		Type type = getCast();
		this->currentToken += 3;
		Expr* expr = unary();
		return new ExprCast(type, expr);
	}
	else
		return postfix();
}

Expr* Parser::postfix()
{
	// TODO: Wire it
	return call();
}

Expr* Parser::call()
{
	Expr* expr = primary();

	if (match(TokenType::OPEN_PAREN))
	{
		Token openParen = consumed();
		std::vector<Expr*> args;
		if (peek().type != TokenType::CLOSE_PAREN)
		{
			do
			{
				args.push_back(parseExpression());
			} while (match(TokenType::COMMA));

		}
		expr = new ExprCall(expr, args, openParen);
		consume(TokenType::CLOSE_PAREN, "Expect ')' after arguments.");
	}

	return expr;
}

Expr* Parser::primary()
{
	Token token = advance();

	switch (token.type)
	{
	case TokenType::TRUE:
		return new ExprLiteral(new Value(true));
	case TokenType::FALSE:
		return new ExprLiteral(new Value(false));
	case TokenType::INTEGER_LITERAL:
		return new ExprLiteral(new Value(token.getInteger()));
	case TokenType::DOUBLE_LITERAL:
		return new ExprLiteral(new Value(token.getDouble()));
	case TokenType::FLOAT_LITERAL:
		return new ExprLiteral(new Value(token.getFloat()));
	case TokenType::STRING_LITERAL:
		return new ExprLiteral(new Value(token.getString()));
	case TokenType::OPEN_PAREN:
	{
		Expr* a = parseExpression();
		if (match(TokenType::CLOSE_PAREN))
			return a;
		else
			return typeError("Expect ')' after a grouping expression.");
	}

	case TokenType::IDENTIFIER:
		return new ExprVariable(token);

	default:
		std::cout << "[Error]Invalid identifier: " << token.getString();
		return nullptr;
	}
}