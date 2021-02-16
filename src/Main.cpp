#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "AstDebugger.hpp"
#include "Common.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "TypeChecker.hpp"
#include "Interpreter.hpp"

void run(const char* filepath);

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "[ERROR] Invalid number of arguments" << std::endl;
        return 1;
    }

    run(argv[1]);

    return 0;
}

void run(const char* filepath)
{
    std::ifstream file(filepath);
    if (file.fail())
    {
        std::cout << "[ERROR] File cannot be openned: '" << filepath << "'\n";
        return;
    }

    std::string file_content;
    {
        std::stringstream file_buffer;
        file_buffer << file.rdbuf();
        file_content = file_buffer.str();
    }
    file.close();

    Scanner scanner(file_content);
    std::vector<Token> tokens = scanner.scanTokens();
    if(scanner.fail())
    {
        std::cout << "Terminated due to scanning error.\n";
        return;
    }
#if LOG_SCANNER
    for (const auto& token : tokens)
    {
        // std::cout << token << std::endl;
        token.showInLine(Logger::BLUE);
    }
#endif
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> root = parser.parse();
    if (parser.fail())
    {
        std::cout << "Terminated due to parse error.\n";
        return;
    }
    AstDebugger debugger(root);
    debugger.debug();

    TypeChecker typeChecker(root);
    typeChecker.check();
    if(typeChecker.fail())
    {
        std::cout << "Terminated due to type error.\n";
        return;
    }
    debugger.showTypes(true);
    debugger.debug();

    Interpreter interpreter(root);
    interpreter.run();
}
