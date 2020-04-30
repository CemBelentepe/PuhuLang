#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Compiler.h"
#include "Chunk.h"
#include "Scanner.h"
#include "VM.h"
#include "Debug.h"

using namespace std;

int func(int a)
{
	return a;
}

void run(string& source)
{
	Scanner scanner(source);
	std::vector<Token> tokenList = scanner.scanTokens();

	VM vm;
	Compiler compiler(tokenList, vm);
	Chunk* chunk = compiler.compile();

#ifdef _DEBUG
	dissableChunk(chunk);
	for (auto& func : vm.globals)
	{
		if (func.second->type.type == ValueType::FUNCTION)
		{
			cout << func.first << ":" << endl;
			dissableChunk((Chunk*)func.second->data);
			cout << "----" << endl;
		}
	}
#endif // DEBUG

 	vm.interpret(chunk);
}

void runRepl()
{
	string line;
	do
	{
		cout << "> ";
		getline(cin, line);
		run(line);
	} while (!line.empty());
}

void runFile(string path)
{
	ifstream file(path);
	if (file.is_open())
	{
		stringstream sourceStream;
		sourceStream << file.rdbuf();
		string source = sourceStream.str();
		run(source);
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		runRepl();
	}
	else if (argc == 2)
	{
		runFile(argv[1]);
	}
	else
	{
		cout << "[Error] Too many arguments" << endl;
	}

	return 0;
}