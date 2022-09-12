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
	while (!isAtEnd() && peek().type != TokenType::EOF_TOKEN)
	{
		// TODO Error handling for declarations should be done in parseDecl or somewhere else idk
		statements.push_back(parseStmt());
	}
	return statements;
}

bool Parser::fail() const
{
	return failed;
}

std::unique_ptr<Stmt> Parser::parseStmt()
{
	// TODO find something better then return nullptr at error

	try
	{
		switch (peek().type)
		{
		case TokenType::OPEN_BRACE:
			return parseStmtBlock();
		case TokenType::IF:
			return parseStmtIf();
		case TokenType::WHILE:
			return parseStmtWhile();
		case TokenType::FOR:
			return parseStmtFor();
		case TokenType::RETURN:
			return parseStmtReturn();
		default:
			return parseStmtExpr();
		}
	}
	catch (parser_stmt_err& e)
	{
		recoverStmt(e);
		// Is this better than `return nullptr` ?
		// if(!isAtEnd()) return parseStmt();
	}

	return nullptr;
}

std::unique_ptr<Stmt> Parser::parseStmtExpr()
{
	auto expr = parseExpr();
	consume(TokenType::SEMI_COLON, "Expected `;` after an expression statement.");
	return std::make_unique<StmtExpr>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::parseStmtBlock()
{
	// TODO add expected consume for dev errors
	Token openBrace = consume(TokenType::OPEN_BRACE, "[DEV] Invalid call to function.");

	std::vector<std::unique_ptr<Stmt>> stmts;

	while (!isAtEnd() && peek().type != TokenType::CLOSE_BRACE)
	{
		size_t savedPos = currentToken;
		try
		{
			TypePtr type;
			try
			{
				type = parseType();
			}
			catch (std::runtime_error& e)
			{
				throw parser_rollback();
			}

			if (peek().type != TokenType::IDENTIFIER)
				throw parser_rollback();
			stmts.push_back(parseDeclVar(type, true));
		}
		catch (parser_stmt_err& e)
		{
			recoverStmt(e);
		}
		catch (parser_rollback& e)
		{
			currentToken = savedPos;
			stmts.push_back(std::move(parseStmt()));
		}
	}

	Token closeBrace = consume(TokenType::CLOSE_BRACE, "Expected `}` after the end of a block statements.");

	return std::make_unique<StmtBlock>(std::move(stmts), openBrace, closeBrace);
}

std::unique_ptr<Stmt> Parser::parseStmtIf()
{
	// TODO add expected consume for dev errors
	consume(TokenType::IF, "[DEV] Invalid call to function.");
	Token paren = consume(TokenType::OPEN_PAREN, "Expected `(` after an `if` keyword.");
	auto cond = parseExpr();
	consume(TokenType::CLOSE_PAREN, "Expected `)` after the end of an `if` condition.");
	auto then = parseStmt();

	std::unique_ptr<Stmt> els = nullptr;
	if (match(TokenType::ELSE))
		els = parseStmt();

	return std::make_unique<StmtIf>(std::move(cond), std::move(then), std::move(els), paren);
}

std::unique_ptr<Stmt> Parser::parseStmtWhile()
{
	// TODO add expected consume for dev errors
	consume(TokenType::WHILE, "[DEV] Invalid call to function.");
	Token paren = consume(TokenType::OPEN_PAREN, "Expected `(` after a `while` keyword.");
	auto cond = parseExpr();
	consume(TokenType::CLOSE_PAREN, "Expected `)` after the end of an `while` condition.");
	auto body = parseStmt();

	return std::make_unique<StmtWhile>(std::move(cond), std::move(body), paren);
}

std::unique_ptr<Stmt> Parser::parseStmtFor()
{
	// TODO add expected consume for dev errors
	consume(TokenType::FOR, "[DEV] Invalid call to function.");
	Token paren = consume(TokenType::OPEN_PAREN, "Expected `(` after a `for` keyword.");

	std::unique_ptr<Stmt> init = nullptr;
	if (peek().type != TokenType::SEMI_COLON)
	{
		size_t savedPos = currentToken;
		try
		{
			TypePtr type;
			try
			{
				type = parseType();
			}
			catch (std::runtime_error& e)
			{
				throw parser_rollback();
			}

			if (peek().type != TokenType::IDENTIFIER)
				throw parser_rollback();
			init = parseDeclVar(type, false);
		}
		catch (parser_rollback& e)
		{
			currentToken = savedPos;
			init = std::make_unique<StmtExpr>(parseExpr());
		}
	}
	consume(TokenType::SEMI_COLON, "Expected `;` after the initializer of `for` statement.");

	std::unique_ptr<Expr> cond = nullptr;
	if (peek().type != TokenType::SEMI_COLON)
		cond = parseExpr();
	consume(TokenType::SEMI_COLON, "Expected `;` after the condition of `for` statement.");

	std::unique_ptr<Expr> fin = nullptr;
	if (peek().type != TokenType::CLOSE_PAREN)
		fin = parseExpr();

	consume(TokenType::CLOSE_PAREN, "Expected `)` after the statements of `for` statement.");

	std::unique_ptr<Stmt> body = parseStmt();

	return std::make_unique<StmtFor>(std::move(init), std::move(cond),
		std::move(fin), std::move(body), paren);
}

std::unique_ptr<Stmt> Parser::parseStmtReturn()
{
	// TODO add expected consume for dev errors
	Token ret = consume(TokenType::RETURN, "[DEV] Invalid call to function.");
	auto expr = parseExpr();
	consume(TokenType::SEMI_COLON, "Expected `;` at the end of `return` statement.");

	return std::make_unique<StmtReturn>(std::move(expr), ret);
}

std::unique_ptr<Stmt> Parser::parseDeclVar(const TypePtr& type, bool consumeSemi)
{
	Token name = consume(TokenType::IDENTIFIER, "Expect an `identifier` after a type for variable declaration.");
	Token eq;
	std::unique_ptr<Expr> init = nullptr;

	if (match(TokenType::EQUAL))
	{
		eq = consumed();
		init = parseExpr();
	}

	if(consumeSemi)
		consume(TokenType::SEMI_COLON, "Expected `;` at the end of a variable declaration");

	return std::make_unique<StmtDeclVar>(type, name, eq, std::move(init));
}

std::unique_ptr<Expr> Parser::parseExpr()
{
	auto expr = parseExprBinary();
	if (expr->instance == Expr::Instance::VarGet)
	{
		Token name = ((ExprVarGet*)(expr.get()))->name;
		if (match(TokenType::EQUAL))
		{
			Token op = consumed();
			auto lhs = parseExpr();
			expr = std::make_unique<ExprVarSet>(name, op, std::move(lhs));
		}
		// TODO add other assignments
	}
	return std::move(expr);
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
	// TODO "--" or "++" should modify the value of the variable
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
	case TokenType::IDENTIFIER:
	{
		return std::make_unique<ExprVarGet>(token);
	}
	default:
	{
		std::stringstream ssErr;
		ssErr << "[ERROR " << peek().line << ":" << peek().col
			  << "] Invalid identifier: \"" + token.getLexeme() + "\".";
		throw parser_stmt_err(ssErr.str());
	}
	}
}

TypePtr Parser::parseType()
{
	TypePtr type = TypeFactory::fromToken(advance());

	if (match(TokenType::CONST))
		type->isConst = true;

	while (match({ TokenType::OPEN_BRACKET, TokenType::STAR }))
	{
		if (consumed().type == TokenType::OPEN_BRACKET)
		{
			consume(TokenType::CLOSE_BRACKET, "Expect `]` after `[` for array type.");
			type = TypeFactory::getArray(type);
		}
		else if (consumed().type == (TokenType::STAR))
		{
			type = TypeFactory::getPointer(type);
		}
		else
		{
			throw parser_stmt_err("[DEV] Token is not added as type!");
		}

		if (match(TokenType::CONST))
			type->isConst = true;
	}

	return type;
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
	if (!isAtEnd() && peek().type == type)
		return advance();

	std::stringstream ssErr;
	if (!isAtEnd())
		ssErr << "[ERROR " << peek().line << ":" << peek().col << "] " << errorMessage;
	else
		ssErr << "[ERROR EOF] " << errorMessage;
	throw parser_stmt_err(ssErr.str());
}

int Parser::getPrecedence(TokenType t)
{
	auto it = precedenceTable.find(t);
	if (it == precedenceTable.end())
		return -1;
	return it->second;
}

bool Parser::match(TokenType type)
{
	if (!isAtEnd() && peek().type == type)
	{
		advance();
		return true;
	}
	return false;
}

bool Parser::match(std::vector<TokenType> types)
{
	TokenType t = peek().type;
	if (!isAtEnd() && std::find(types.begin(), types.end(), t) != types.end())
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

void Parser::recoverStmt(parser_stmt_err& e)
{
	failed = true;
	std::cerr << e.what() << std::endl;

	while (!isAtEnd())
	{
		switch (peek().type)
		{
		case TokenType::SEMI_COLON:
			advance();
			return;
		case TokenType::OPEN_BRACE:
		case TokenType::IF:
		case TokenType::WHILE:
		case TokenType::FOR:
		case TokenType::RETURN:
			return;
		default:
			break;
		}
		advance();
	}
}

const std::unordered_map<TokenType, int> Parser::precedenceTable = {
	{ TokenType::OR, 1 },
	{ TokenType::AND, 2 },
	{ TokenType::BIT_OR, 10 },
	{ TokenType::BIT_XOR, 11 },
	{ TokenType::BIT_AND, 12 },
	{ TokenType::EQUAL_EQUAL, 20 },
	{ TokenType::BANG_EQUAL, 20 },
	{ TokenType::LESS, 30 },
	{ TokenType::LESS_EQUAL, 30 },
	{ TokenType::GREAT, 30 },
	{ TokenType::GREAT_EQUAL, 30 },
	{ TokenType::BITSHIFT_LEFT, 40 },
	{ TokenType::BITSHIFT_RIGHT, 40 },
	{ TokenType::PLUS, 50 },
	{ TokenType::MINUS, 50 },
	{ TokenType::STAR, 60 },
	{ TokenType::SLASH, 60 },
	{ TokenType::MODULUS, 60 },
};
