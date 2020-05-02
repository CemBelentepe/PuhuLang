#pragma once
#include <vector>
#include <unordered_map>
#include "Scanner.h"
#include "Chunk.h"
#include "VM.h"

class Compiler
{
public:
	Compiler(std::vector<Token>& tokens, VM& vm);
	Chunk* compile();

private:
	std::vector<Token>& tokens;
	size_t currentToken;
	VM& vm;
	Chunk* compilingChunk;
	int scopeDepth;
	int frame;
	std::vector<LocalVariable> locals;
	std::unordered_map<std::string, Value*> globals;

	void addNatives();

	void firstPass();
	void parseDeclarations();
	void parseFunctionDeclaration();
	void parseGlobalDeclaration(DataType type, Token name);
	void parseClassDeclaration() {}
	DataType parseTypeName();
	bool isTypeName(Token& token);
	ValueType getDataType(Token& token);
	void consume(TokenType type, const char* message);
	void consumeNext(TokenType type, const char* message);

	void statement();
	void decleration();
	void variableDecleration(DataType type);
	void functionDecleration(DataType type);
	void block();
	void ifStatement();
	void whileStatement();
	void forStatement();

	uint8_t identifierConstant(std::string& name);
	void beginScope();
	void endScope();
	void addLocal(DataType type, std::string name, int depth);

	DataType compileExpression();
	DataType logic_or();
	DataType logic_and();
	DataType bit_or();
	DataType bit_xor();
	DataType bit_and();
	DataType equality();
	DataType comparison();
	DataType bitshift();
	DataType addition();
	DataType multiplication();
	DataType prefix();
	DataType unary();
	DataType postfix();
	DataType call();
	DataType primary();

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
	DataType getCast();
	void addCode(OpCode code);
	void addCode(OpCode code1, uint8_t code2);
	void addCode(OpCode code1, uint8_t code2, uint8_t code3);
	void addCode(uint8_t code);
	void emitCast(DataType from, DataType to);
	void emitCast(DataType from, DataType to, size_t pos);

	inline DataType typeError(const char* message) const;
	inline void error(const char* message);
	inline void errorAtToken(const char* message);

	void panic();
};

