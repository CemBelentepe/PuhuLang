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
private:
	class parser_rollback : public std::exception
	{
	public:
		explicit parser_rollback() : std::exception()
		{
		}
	};

	class parser_stmt_err : public std::runtime_error
	{
	public:
		explicit parser_stmt_err(const std::string& msg)
			: std::runtime_error(msg)
		{
		}
	};

public:
	explicit Parser(std::vector<Token> tokens);

	std::vector<std::unique_ptr<Stmt>> parse();
	[[nodiscard]] bool fail() const;

private:
	std::unique_ptr<Stmt> parseDecl();
	std::unique_ptr<StmtDeclFunc> parseDeclFunc(const TypePtr& type);
	std::unique_ptr<StmtDeclVar> parseDeclVar(const TypePtr& type);

	std::unique_ptr<Stmt> 		parseStmt();
	std::unique_ptr<StmtExpr> 	parseStmtExpr();
	std::unique_ptr<StmtBlock> 	parseStmtBlock();
	std::unique_ptr<StmtIf> 	parseStmtIf();
	std::unique_ptr<StmtWhile> 	parseStmtWhile();
	std::unique_ptr<StmtFor> 	parseStmtFor();
	std::unique_ptr<StmtReturn> parseStmtReturn();

	std::unique_ptr<Expr> parseExpr();
	std::unique_ptr<Expr> parseExprBinary();
	std::unique_ptr<Expr> parseExprBinaryHelper(std::unique_ptr<Expr> lhs, int precedence);
	std::unique_ptr<Expr> parseExprPrefix();
	std::unique_ptr<Expr> parseExprUnary();
	std::unique_ptr<Expr> parseExprCall();
	std::unique_ptr<Expr> parseExprPrimary();

	TypePtr parseType();
	Token peek(int n = 0);

	Token advance();
	Token consume(TokenType type, const std::string& errorMessage);
	[[nodiscard]] static int getPrecedence(TokenType t);
	bool match(TokenType type);
	bool match(std::vector<TokenType> types);
	[[nodiscard]] Token consumed() const;
	[[nodiscard]] bool isAtEnd() const;

	void recoverStmt(parser_stmt_err& e);
	static bool isRValue(Expr* expr);

private:
	std::vector<Token> tokens;
	size_t currentToken;
	bool failed;
	static const std::unordered_map<TokenType, int> precedenceTable;
};
