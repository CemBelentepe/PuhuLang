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
		try
		{
			statements.push_back(parseDecl());
		}
		catch (parser_stmt_err& e)
		{
			failed = true;
			std::cout << e.what() << std::endl;
		}
	}
	return statements;
}

bool Parser::fail() const
{
	return failed;
}

std::unique_ptr<Stmt> Parser::parseDecl()
{
	TypePtr type = parseType();
	if (peek(1).type == TokenType::OPEN_PAREN)
	{
		return parseDeclFunc(type);
	}
	else
	{
		return parseDeclVar(type);
	}
}

std::unique_ptr<StmtDeclFunc> Parser::parseDeclFunc(const TypePtr& type)
{
	Token name = consume(TokenType::IDENTIFIER, "Expect an `identifier` after a type for variable declaration.");
	Token paren = consume(TokenType::OPEN_PAREN, "[DEV] Invalid call to function `parseDeclFunc`.");

	std::vector<std::tuple<TypePtr, Token>> params;
	std::vector<TypePtr> paramTypes;
	if (peek().type != TokenType::CLOSE_PAREN)
	{
		do
		{
			TypePtr paramType = parseType();
			paramTypes.push_back(paramType);
			Token paramName = consume(TokenType::IDENTIFIER, "Expected an `identifier` as a function argument name.");
			params.emplace_back(paramType, paramName);
		}while (match(TokenType::COMMA));
	}
	consume(TokenType::CLOSE_PAREN, "Expected a `)` after the arguments of a call.");
	auto body = parseStmtBlock();

	auto funcType = TypeFactory::getFunction(type, paramTypes);

	return std::make_unique<StmtDeclFunc>(funcType, name, paren, params, std::move(body));
}

std::unique_ptr<StmtDeclVar> Parser::parseDeclVar(const TypePtr& type)
{
	Token name = consume(TokenType::IDENTIFIER, "Expect an `identifier` after a type for variable declaration.");
	Token eq;
	std::unique_ptr<Expr> init = nullptr;

	if (match(TokenType::EQUAL))
	{
		eq = consumed();
		init = parseExpr();
	}

	consume(TokenType::SEMI_COLON, "Expected `;` at the end of a variable declaration");

	return std::make_unique<StmtDeclVar>(type, name, eq, std::move(init));
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

std::unique_ptr<StmtExpr> Parser::parseStmtExpr()
{
	auto expr = parseExpr();
	Token semicolon = consume(TokenType::SEMI_COLON, "Expected `;` after an expression statement.");
	return std::make_unique<StmtExpr>(std::move(expr), semicolon);
}

std::unique_ptr<StmtBlock> Parser::parseStmtBlock()
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
			stmts.push_back(parseDeclVar(type));
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

std::unique_ptr<StmtIf> Parser::parseStmtIf()
{
	// TODO add expected consume for dev errors
	Token ifTok = consume(TokenType::IF, "[DEV] Invalid call to function.");
	Token paren = consume(TokenType::OPEN_PAREN, "Expected `(` after an `if` keyword.");
	auto cond = parseExpr();
	consume(TokenType::CLOSE_PAREN, "Expected `)` after the end of an `if` condition.");
	auto then = parseStmt();

	std::unique_ptr<Stmt> els = nullptr;
	if (match(TokenType::ELSE))
		els = parseStmt();

	return std::make_unique<StmtIf>(ifTok, std::move(cond), std::move(then), std::move(els), paren);
}

std::unique_ptr<StmtWhile> Parser::parseStmtWhile()
{
	// TODO add expected consume for dev errors
	Token whileTok = consume(TokenType::WHILE, "[DEV] Invalid call to function.");
	Token paren = consume(TokenType::OPEN_PAREN, "Expected `(` after a `while` keyword.");
	auto cond = parseExpr();
	consume(TokenType::CLOSE_PAREN, "Expected `)` after the end of an `while` condition.");
	auto body = parseStmt();

	return std::make_unique<StmtWhile>(whileTok, std::move(cond), std::move(body), paren);
}

std::unique_ptr<StmtFor> Parser::parseStmtFor()
{
	// TODO add expected consume for dev errors
	Token forTok = consume(TokenType::FOR, "[DEV] Invalid call to function.");
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
			init = parseDeclVar(type);
		}
		catch (parser_rollback& e)
		{
			currentToken = savedPos;
			init = parseStmtExpr();
		}
	}
	else
	{
		consume(TokenType::SEMI_COLON, "Expected `;` after the initializer of `for` statement.");
	}

	std::unique_ptr<Expr> cond = nullptr;
	if (peek().type != TokenType::SEMI_COLON)
		cond = parseExpr();
	consume(TokenType::SEMI_COLON, "Expected `;` after the condition of `for` statement.");

	std::unique_ptr<Expr> fin = nullptr;
	if (peek().type != TokenType::CLOSE_PAREN)
		fin = parseExpr();

	consume(TokenType::CLOSE_PAREN, "Expected `)` after the statements of `for` statement.");

	std::unique_ptr<Stmt> body = parseStmt();

	return std::make_unique<StmtFor>(forTok, std::move(init), std::move(cond),
			std::move(fin), std::move(body), paren);
}

std::unique_ptr<StmtReturn> Parser::parseStmtReturn()
{
	// TODO add expected consume for dev errors
	Token ret = consume(TokenType::RETURN, "[DEV] Invalid call to function.");
	auto expr = parseExpr();
	consume(TokenType::SEMI_COLON, "Expected `;` at the end of `return` statement.");

	return std::make_unique<StmtReturn>(std::move(expr), ret);
}

std::unique_ptr<Expr> Parser::parseExpr()
{
	auto expr = parseExprBinary();
	if (expr->instance == Expr::Instance::VarGet || expr->instance == Expr::Instance::Deref)
	{
		if (match(TokenType::EQUAL))
		{
			Token op = consumed();
			auto rhs = parseExpr();
			expr = std::make_unique<ExprVarSet>(std::move(expr), op, std::move(rhs));
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
	else if (match(TokenType::BIT_AND))
	{
		Token op = consumed();
		std::unique_ptr<Expr> rhs = parseExprUnary();
		if(isLValue(rhs.get()))
		{
			return std::make_unique<ExprAddrOf>(std::move(rhs), op);
		}
		else
		{
			std::stringstream ssErr;
			ssErr << "[ERROR " << op.line << ":" << op.col << "] Only an lvalue can have an address.";
			throw parser_stmt_err(ssErr.str());
		}
	}
	else if (match(TokenType::STAR))
	{
		Token op = consumed();
		std::unique_ptr<Expr> rhs = parseExprUnary();
		return std::make_unique<ExprDeref>(std::move(rhs), op);
	}
	else
	{
		return parseExprCall();
	}
}

std::unique_ptr<Expr> Parser::parseExprCall()
{
	std::unique_ptr<Expr> expr = parseExprPrimary();
	if (match(TokenType::OPEN_PAREN))
	{
		Token paren = consumed();
		std::vector<std::unique_ptr<Expr>> args;
		if (peek().type != TokenType::CLOSE_PAREN)
		{
			args.push_back(parseExpr());
			while (match(TokenType::COMMA))
			{
				args.push_back(parseExpr());
			}
		}
		consume(TokenType::CLOSE_PAREN, "Expected a `)` after the arguments of a call.");
		expr = std::make_unique<ExprCall>(std::move(expr), paren, std::move(args));
	}
	return std::move(expr);
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
	case TokenType::NEW:
	{
		TypePtr type = parseType();
		return std::make_unique<ExprNew>(token, type);
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
			// TODO Add support for sized arrays
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

Token Parser::peek(int n)
{
	return tokens[currentToken + n];
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

bool Parser::isLValue(Expr* expr)
{
	static std::vector<Expr::Instance> lvalues = {
		Expr::Instance::VarGet,
	};

	auto it = std::find(lvalues.begin(), lvalues.end(), expr->instance);
	return it != lvalues.end();
}

const std::unordered_map<TokenType, int> Parser::precedenceTable = {
		{ TokenType::OR,             1 },
		{ TokenType::AND,            2 },
		{ TokenType::BIT_OR,         10 },
		{ TokenType::BIT_XOR,        11 },
		{ TokenType::BIT_AND,        12 },
		{ TokenType::EQUAL_EQUAL,    20 },
		{ TokenType::BANG_EQUAL,     20 },
		{ TokenType::LESS,           30 },
		{ TokenType::LESS_EQUAL,     30 },
		{ TokenType::GREAT,          30 },
		{ TokenType::GREAT_EQUAL,    30 },
		{ TokenType::BITSHIFT_LEFT,  40 },
		{ TokenType::BITSHIFT_RIGHT, 40 },
		{ TokenType::PLUS,           50 },
		{ TokenType::MINUS,          50 },
		{ TokenType::STAR,           60 },
		{ TokenType::SLASH,          60 },
		{ TokenType::MODULUS,        60 },
};
