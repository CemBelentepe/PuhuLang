#pragma once

#include "Scanner.h"
#include "Value.hpp"
#include "AST.h"

#include <vector>
#include <unordered_map>

class Expr;

class Parser
{
public:
	Parser();
	StmtCompUnit* parseUnit(std::vector<Token> tokens);
	void parseUserDefinedTypes(std::vector<Token> tokens);
	std::unordered_map<std::string, Value*> globals;
	int depth;
	bool cont;

private:
	std::vector<Token> tokens;
	size_t currentToken;
	std::unordered_map<std::string, std::shared_ptr<TypeStruct>> userTypes;

	std::shared_ptr<Type> parseTypeName();
	bool isTypeName(Token& token);
	TypeTag getDataType();
	void consume(TokenType type, const char* message);
	void consumeNext(TokenType type, const char* message);

	inline Token& advance()
	{
		return this->tokens[this->currentToken++];
	}
	inline Token& consumed()
	{
		return this->tokens[this->currentToken - 1];
	}
	inline Token& peek()
	{
		return this->tokens[this->currentToken];
	}
	inline Token& peekNext()
	{
		return this->tokens[this->currentToken + 1];
	}
	bool match(TokenType type);
	bool match(std::vector<TokenType> types);
	bool matchCast();
	std::shared_ptr<Type> getCast();

	inline Expr* typeError(const char* message);
	inline void error(const char* message);
	inline void errorAtToken(const char* message);
	void panic();

	Stmt* decleration();
	Stmt* structDecleration();
	Stmt* variableDecleration(std::shared_ptr<Type> type);
	Stmt* functionDecleration(std::shared_ptr<Type> type);
	Stmt* statement();
	Stmt* block();
	Stmt* ifStatement();
	Stmt* forStatement();
	Stmt* whileStatement();

	Expr* parseExpression();
	Expr* assignment();
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