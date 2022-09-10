#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.h"
#include "Token.h"
#include "Parser.h"
#include "Stmt.h"
#include "AstDebugger.h"
#include "TypeChecker.h"
#include "Interpreter.h"

int run(const std::string& fileName);

int main(int argc, char** argv)
{
	if (argc != 2) return EXIT_FAILURE;

	return run(argv[1]);
}

int run(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::stringstream fileContent;

	// TODO handle file error
	fileContent << file.rdbuf();

	Scanner scanner(std::move(fileContent.str()));
	std::vector<Token> tokens = scanner.scan();

	// TODO add a compiler flag
	if (false)
	{
		std::cout << "Tokens: " << std::endl;
		for (auto& tok: tokens)
			std::cout << tok.showInfo() << std::endl;
		std::cout << std::endl;
	}

	Parser parser(tokens);
	std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

	if (true)
	{
		std::cout << "AST: " << std::endl;
		AstDebugger debugger(statements, std::cout);
		debugger.debug();
		std::cout << std::endl;
	}

	TypeChecker typeChecker(statements);
	typeChecker.check();

	if(true)
	{
		std::cout << "Typed AST: " << std::endl;
		AstDebugger debugger(statements, std::cout);
		debugger.setShowTypes(true);
		debugger.debug();
		std::cout << std::endl;
	}

	std::cout << "Interpreter: " << std::endl;
	Interpreter interpreter(statements, std::cout);
	interpreter.run();

	return EXIT_SUCCESS;
}
