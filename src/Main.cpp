#define DEBUG

#ifdef DEBUG
// #define DEBUG_TOKENS
#define DEBUG_BARE_AST
#define DEBUG_AST
#define DEBUG_IR
#define DEBUG_CODE
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "CodeGen.hpp"
#include "IRGen.hpp"
#include "Parser.h"
#include "Scanner.h"
#include "TypeChecker.hpp"
#include "VM.h"

#ifdef DEBUG
#include "Debug.h"
#endif

void run(const char* path)
{
    std::ifstream file(path);
    if (file.fail())
    {
        std::cout << "[Error] Unable to open file: " << path << std::endl;
        return;
    }

    std::stringstream sourceStream;
    sourceStream << file.rdbuf();
    std::string source = sourceStream.str();
    
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    file.close();

#ifdef DEBUG_TOKENS
    debugTokens(tokens);
#endif

    Parser parser(tokens);
    std::vector<Stmt*> root = parser.parse();

#ifdef DEBUG_BARE_AST
    debugAST(root);
#endif

    TypeChecker typeChecker(root, parser.globals);

#ifdef DEBUG_AST
    debugAST(root);
#endif

    IRGen irGen(root, parser.globals);
    std::vector<IRChunk*> irChunks = irGen.generateIR();

    for(auto& stmt : root)
        delete stmt;
    root.clear();

#ifdef DEBUG_IR
    for (auto& irc : irChunks)
    {
        std::cout << irc->name << ":\n";
        debugInstructions(irc);
        std::cout << std::endl;
    }
#endif

    CodeGen codegen(parser.globals);
    for (auto& irc : irChunks)
        codegen.generateCode(irc);

#ifdef DEBUG_CODE
    for (auto& irc : irChunks)
    {
        std::cout << irc->name << "@" << irc->chunk << ":\n";
        dissambleChunk(irc->chunk);
        std::cout << std::endl;
    }
#endif

    std::vector<Chunk*> chunks;
    for(auto& irc : irChunks)
    {
        chunks.push_back(irc->chunk);
        delete irc;
    }
    irChunks.clear();

    VM vm(codegen.getGlobals());
    vm.interpret(chunks[0]);
}

int main(int argc, char* argv[])
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