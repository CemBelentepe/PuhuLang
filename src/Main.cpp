#define DEBUG

#ifdef DEBUG
	// #define DEBUG_TOKENS
	#define DEBUG_BARE_AST
	#define DEBUG_AST
	#define DEBUG_IR
	#define DEBUG_CODE
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Scanner.h"
#include "Parser.h"
#include "TypeChecker.hpp"
#include "IRGen.hpp"
#include "CodeGen.hpp"
#include "VM.h"

#ifdef DEBUG
#include "Debug.h"
#endif

void run(const char *path)
{
	std::ifstream file(path);
	if (file.fail())
	{
		std::cout << "[Error] Unsable to open file: " << path << std::endl;
		return;
	}

	std::stringstream sourceStream;
	sourceStream << file.rdbuf();
	std::string source = sourceStream.str();

	Scanner scanner(source);
	std::vector<Token> tokens = scanner.scanTokens();

#ifdef DEBUG_TOKENS
	debugTokens(tokens);
#endif

	Parser parser(tokens);
	Expr* expr = parser.parse();

#ifdef DEBUG_BARE_AST
	debugAST(expr);
#endif

	TypeChecker typeChecker(expr);

#ifdef DEBUG_AST
	debugAST(expr);
#endif

	IRGen irGen(expr);
	IRChunk* mainIR = irGen.generateIR();

#ifdef DEBUG_IR
	debugInstructions(mainIR);
#endif

	CodeGen codegen(mainIR);
	Chunk* chunk = codegen.generateCode();

#ifdef DEBUG_CODE
	dissambleChunk(chunk);
#endif

	VM vm;
	vm.interpret(chunk);
}

int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		run(argv[1]);
	}
	else
	{
		std::cout << "[Error] Too many arguments." << std::endl;
	}

	return 0;
}