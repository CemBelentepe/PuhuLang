#pragma once

#include <vector>
#include "Scanner.h"
#include "Value.h"

class Expr;

class Parser
{
public:
	Parser(std::vector<Token>& tokens);
	Expr* parse();

private:
	std::vector<Token>& tokens;
	size_t currentToken;

	Type parseTypeName();
	bool isTypeName(Token& token);
	TypeTag getDataType(Token& token);
	void consume(TokenType type, const char* message);
	void consumeNext(TokenType type, const char* message);

	inline Token& advance()
	{
		return this->tokens[this->currentToken++];
	}
	inline Token& consumed() const
	{
		return this->tokens[this->currentToken - 1];
	}
	inline Token& peek() const
	{
		return this->tokens[this->currentToken];
	}
	inline Token& peekNext() const
	{
		return this->tokens[this->currentToken + 1];
	}
	bool match(TokenType type);
	bool matchCast();
	Type getCast();

	inline Expr* typeError(const char* message) const;
	inline void error(const char* message);
	inline void errorAtToken(const char* message);
	void panic();

	// Stmt* decleration();
	// Stmt* variableDecleration(DataType type);
	// Stmt* functionDecleration(DataType type);
	// Stmt* statement();
	// Stmt* block();

	Expr* parseExpression();
	Expr* logic_or();
	Expr* logic_and();
	Expr* bit_or();
	Expr* bit_xor();
	Expr* bit_and();
	Expr* equality();
	Expr* comparison();
	Expr* bitshift();
	Expr* addition();
	Expr* multiplication();
	Expr* prefix();
	Expr* unary();
	Expr* postfix();
	Expr* call();
	Expr* primary();
};