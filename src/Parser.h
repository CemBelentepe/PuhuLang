//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <vector>
#include <unordered_map>
#include "Token.h"
#include "Stmt.h"

class Parser
{
public:
	explicit Parser(std::vector<Token>  tokens);

	std::vector<std::unique_ptr<Stmt>> parse();
	[[nodiscard]] bool fail() const;

private:
	std::unique_ptr<Stmt> parseStmt();
	std::unique_ptr<Stmt> parseStmtExpr();

	std::unique_ptr<Expr> parseExpr();
	std::unique_ptr<Expr> parseExprBinary();
	std::unique_ptr<Expr> parseExprBinaryHelper(std::unique_ptr<Expr> lhs, int precedence);
	std::unique_ptr<Expr> parseExprPrefix();
	std::unique_ptr<Expr> parseExprUnary();
	std::unique_ptr<Expr> parseExprPrimary();

	Token peek();
	Token advance();
	Token consume(TokenType type, const std::string& errorMessage);
	[[nodiscard]] static int getPrecedence (TokenType t) ;
	bool match(TokenType type);
	bool match(std::vector<TokenType> types);
	[[nodiscard]] Token consumed() const;

private:
	std::vector<Token> tokens;
	size_t currentToken;

};
