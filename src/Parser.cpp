//
// Created by cembelentepe on 08/09/22.
//

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <sstream>
#include "Parser.h"

Parser::Parser(std::vector<Token> tokens)
		: tokens(std::move(tokens)), currentToken(0), failed(false)
{
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
	std::vector<std::unique_ptr<Stmt>> statements;
	while (peek().type != TokenType::EOF_TOKEN && !isAtEnd())
	{
		try
		{
			statements.push_back(parseStmt());
		}
		catch (std::runtime_error& e)
		{
			failed = true;
			recover();
			std::cerr << e.what() << std::endl;
		}
	}
	return statements;
}

bool Parser::fail() const
{
	return failed;
}

std::unique_ptr<Stmt> Parser::parseStmt()
{
	return parseStmtExpr();
}

std::unique_ptr<Stmt> Parser::parseStmtExpr()
{
	auto expr = parseExpr();
	Token sm = consume(TokenType::SEMI_COLON, "Expected ';' after an expression statement.");
	return std::make_unique<StmtExpr>(std::move(expr), sm);
}

std::unique_ptr<Expr> Parser::parseExpr()
{
	return parseExprBinary();
}

std::unique_ptr<Expr> Parser::parseExprBinary()
{
	return parseExprBinaryHelper(parseExprPrefix(), 0);
}

std::unique_ptr<Expr> Parser::parseExprBinaryHelper(std::unique_ptr<Expr> lhs, int precedence)
{
	int tokenPrecedence = getPrecedence(peek().type);
	while (tokenPrecedence >= precedence)
	{
		Token op = advance();
		std::unique_ptr<Expr> rhs = parseExprPrefix();

		if (getPrecedence(peek().type) > tokenPrecedence)
			rhs = parseExprBinaryHelper(std::move(rhs), tokenPrecedence + 1);

		lhs = std::make_unique<ExprBinary>(op, std::move(lhs), std::move(rhs));
		tokenPrecedence = getPrecedence(peek().type);
	}

	return lhs;
}

std::unique_ptr<Expr> Parser::parseExprPrefix()
{
	if (match({ TokenType::MINUS, TokenType::PLUS, TokenType::MINUS_MINUS, TokenType::PLUS_PLUS }))
	{
		Token op = consumed();
		std::unique_ptr<Expr> rhs = parseExprUnary();
		return std::make_unique<ExprUnary>(std::move(rhs), op);
	}
	else
	{
		return parseExprUnary();
	}
}

std::unique_ptr<Expr> Parser::parseExprUnary()
{
	if (match({ TokenType::BANG, TokenType::TILDE }))
	{
		Token op = consumed();
		std::unique_ptr<Expr> rhs = parseExprUnary();
		return std::make_unique<ExprUnary>(std::move(rhs), op);
	}
	else
	{
		return parseExprPrimary();
	}
}

std::unique_ptr<Expr> Parser::parseExprPrimary()
{
	Token token = advance();

	switch (token.type)
	{
	case TokenType::TRUE:
	case TokenType::FALSE:
	case TokenType::INTEGER_LITERAL:
	case TokenType::FLOAT_LITERAL:
	case TokenType::CHAR_LITERAL:
	case TokenType::DOUBLE_LITERAL:
	case TokenType::STRING_LITERAL:
		return std::make_unique<ExprLiteral>(token);
	case TokenType::OPEN_PAREN:
	{
		std::unique_ptr<Expr> inside = parseExpr();
		consume(TokenType::CLOSE_PAREN, "Expect ')' after grouping expression.");
		return std::move(inside);
	}
	default:
	{
		std::stringstream ssErr;
		ssErr << "[ERROR " << peek().line << ":" << peek().col << "] Invalid identifier: \"" + token.getLexeme() + "\".";
		throw std::runtime_error(ssErr.str());
	}
	}
}

Token Parser::peek()
{
	return tokens[currentToken];
}

Token Parser::advance()
{
	return tokens[currentToken++];
}

Token Parser::consume(TokenType type, const std::string& errorMessage)
{
	if (peek().type == type)
		return advance();

	std::stringstream ssErr;
	ssErr << "[ERROR " << peek().line << ":" << peek().col << "] " << errorMessage;
	throw std::runtime_error(ssErr.str());
}

int Parser::getPrecedence(TokenType t)
{
	switch (t)
	{
	case TokenType::OR:
		return 1;
	case TokenType::AND:
		return 2;

		// Bitwise
	case TokenType::BIT_OR:
		return 10;
	case TokenType::BIT_XOR:
		return 11;
	case TokenType::BIT_AND:
		return 12;

		// Equality
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG_EQUAL:
		return 20;

		// Comparison
	case TokenType::LESS:
	case TokenType::LESS_EQUAL:
	case TokenType::GREAT:
	case TokenType::GREAT_EQUAL:
		return 30;

		// Bitshift
	case TokenType::BITSHIFT_LEFT:
	case TokenType::BITSHIFT_RIGHT:
		return 40;

		// Addition
	case TokenType::PLUS:
	case TokenType::MINUS:
		return 50;

		// Multiplication
	case TokenType::STAR:
	case TokenType::SLASH:
	case TokenType::MODULUS:
		return 60;

	default:
		return -1;
	}
}

bool Parser::match(TokenType type)
{
	if (peek().type == type)
	{
		advance();
		return true;
	}
	return false;
}

bool Parser::match(std::vector<TokenType> types)
{
	TokenType t = peek().type;
	if (std::find(types.begin(), types.end(), t) != types.end())
	{
		advance();
		return true;
	}

	return false;
}

Token Parser::consumed() const
{
	return tokens[currentToken - 1];
}

bool Parser::isAtEnd() const
{
	return currentToken >= tokens.size();
}

void Parser::recover()
{
	TokenType c = advance().type;
	while (!isAtEnd())
	{
		if (c == TokenType::SEMI_COLON) break;

		c = advance().type;
	}
}

