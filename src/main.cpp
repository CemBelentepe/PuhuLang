#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.h"
#include "Token.h"

int run(const std::string& fileName);

int main(int argc, char** argv)
{
	if(argc != 2) return EXIT_FAILURE;

	return run(argv[1]);
}

int run(const std::string& fileName)
{
	std::ifstream file(fileName);
	std::stringstream fileContent;
	fileContent << file.rdbuf();

	Scanner scanner(fileContent.str());
	std::vector<Token> tokens = scanner.scan();

	return EXIT_SUCCESS;
}
