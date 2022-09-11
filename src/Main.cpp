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

int run(const std::string& filepath);

int main(int argc, char** argv)
{
	if (argc != 2) return EXIT_FAILURE;

	return run(argv[1]);
}

int run(const std::string& filepath)
{
	std::ifstream file(filepath);
	std::stringstream fileContent;

	if (file.fail())
	{
		std::cout << "[ERROR] File cannot be opened: '" << filepath << "'\n";
		return EXIT_FAILURE;
	}

	fileContent << file.rdbuf();
	file.close();

	Scanner scanner(fileContent.str());
	std::vector<Token> tokens = scanner.scan();

	if (scanner.fail())
	{
		std::cout << "Terminated due to a lexing failure." << std::endl;
		return EXIT_FAILURE;
	}

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

	if (parser.fail())
	{
		std::cout << "Terminated due to a parsing failure." << std::endl;
		return EXIT_FAILURE;
	}

	if (false)
	{
		std::cout << "AST: " << std::endl;
		AstDebugger debugger(statements, std::cout);
		debugger.debug();
		std::cout << std::endl;
	}

	TypeChecker typeChecker(statements);
	typeChecker.check();

	if (typeChecker.fail())
	{
		std::cout << "Terminated due to a type checking failure." << std::endl;
		return EXIT_FAILURE;
	}

	if (false)
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

	if (interpreter.fail())
	{
		std::cout << "Terminated due to a runtime failure." << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
