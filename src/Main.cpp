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

enum class TargetPlatform
{
    Interpret,
    VMOutput,
    M0Stack,
    M0Register
};

struct BuildMode
{
    std::vector<std::string> files;
    std::string targetPath;
    TargetPlatform target;
    
    bool debug_tokens;
    bool debug_ast_bare;
    bool debug_ast;
    bool debug_ir;
    bool debug_code;
};

void run(char** paths, int n)
{
    Parser parser;
    std::vector<Stmt*> root;
    std::vector<std::string> ss;
    std::vector<std::vector<Token>> tokenList;
    ss.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::ifstream file(paths[i]);
        if (file.fail())
        {
            std::cout << "[Error] Unable to open file: " << paths[i] << std::endl;
            return;
        }

        std::stringstream sourceStream;
        sourceStream << file.rdbuf();
        ss[i] = sourceStream.str();

        Scanner scanner(ss[i]);
        tokenList.push_back(scanner.scanTokens());
        file.close();

#ifdef DEBUG_TOKENS
        debugTokens(tokenList[i]);
#endif

        parser.parseUserDefinedTypes(tokenList[i]);
    }

    for(int i = 0; i < n; i++)
    {
        root.push_back(parser.parseUnit(tokenList[i]));
    }
    if(!parser.cont) return;


#ifdef DEBUG_BARE_AST
    debugAST(root);
#endif

    TypeChecker typeChecker(root, parser.globals);

    if(!typeChecker.cont) return;

#ifdef DEBUG_AST
    debugAST(root);
#endif

    IRGen irGen(root, parser.globals);
    std::vector<IRChunk*> irChunks = irGen.generateIR();

    for (auto& stmt : root)
        delete stmt;
    root.clear();

    if(!irGen.cont) return;

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
    for (auto& irc : irChunks)
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
    if (argc >= 2)
    {
        run(&argv[1], argc - 1);
    }
    else
    {
        std::cout << "[Error] Too many arguments." << std::endl;
    }

    return 0;
}