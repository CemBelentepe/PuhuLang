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

#include "Debug.h"

size_t EnvNamespace::currentPos = 0;

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

    bool debug_tokens = false;
    bool debug_ast_bare = false;
    bool debug_ast = false;
    bool debug_ir = false;
    bool debug_code = false;
};

BuildMode parseArgs(int argc, char** argv);
void run(BuildMode buildMode);

int main(int argc, char* argv[])
{
    if (argc >= 2)
    {
        BuildMode buildMode = parseArgs(argc, argv);
        if (buildMode.target == TargetPlatform::Interpret)
            run(buildMode);
        else
        {
            std::cout << "[ERROR] Invalid build target." << std::endl;
        }
    }
    else
    {
        std::cout << "[Error] Not enough arguments." << std::endl;
    }

    return 0;
}

BuildMode parseArgs(int argc, char** argv)
{
    BuildMode buildMode;
    buildMode.target = TargetPlatform::Interpret;

    int parse_mode = 0;

    for (int i = 1; i < argc; i++)
    {
        if (parse_mode == 0)
        {
            if (strcmp(argv[i], "-debug_token") == 0)
                buildMode.debug_tokens = true;
            else if (strcmp(argv[i], "-debug_ast_bare") == 0)
                buildMode.debug_ast_bare = true;
            else if (strcmp(argv[i], "-debug_ast") == 0)
                buildMode.debug_ast = true;
            else if (strcmp(argv[i], "-debug_ir") == 0)
                buildMode.debug_ir = true;
            else if (strcmp(argv[i], "-debug_code") == 0)
                buildMode.debug_code = true;
            else if (strcmp(argv[i], "-interpret") == 0)
                buildMode.target = TargetPlatform::Interpret;
            else if (strcmp(argv[i], "-vmcode") == 0)
                buildMode.target = TargetPlatform::VMOutput;
            else if (strcmp(argv[i], "-m0stack") == 0)
                buildMode.target = TargetPlatform::M0Stack;
            else if (strcmp(argv[i], "-m0register") == 0)
                buildMode.target = TargetPlatform::M0Register;
            else
                parse_mode++;
        }

        if (parse_mode == 1)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                parse_mode++;
                i++;
            }
            else
            {
                buildMode.files.push_back(argv[i]);
            }
        }

        if (parse_mode == 2)
        {
            buildMode.targetPath = std::string(argv[i]);
            break;
        }
    }

    return buildMode;
}

void run(BuildMode buildMode)
{
    int n = buildMode.files.size();
    Parser parser;
    std::vector<Stmt*> root;
    std::vector<std::string> ss;
    std::vector<std::vector<Token>> tokenList;
    ss.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::ifstream file(buildMode.files[i]);
        if (file.fail())
        {
            std::cout << "[Error] Unable to open file: " << buildMode.files[i] << std::endl;
            return;
        }

        std::stringstream sourceStream;
        sourceStream << file.rdbuf();
        ss[i] = sourceStream.str();

        Scanner scanner(ss[i]);
        tokenList.push_back(scanner.scanTokens());
        file.close();

        if (buildMode.debug_tokens)
            debugTokens(tokenList[i]);

        parser.parseUserDefinedTypes(tokenList[i]);
    }

    for (int i = 0; i < n; i++)
    {
        root.push_back(parser.parseUnit(tokenList[i]));
    }
    if (!parser.cont)
        return;

    if (buildMode.debug_ast_bare)
        debugAST(root);

    TypeChecker typeChecker(root, parser.allNamespaces);

    if (!typeChecker.cont)
        return;

    if (buildMode.debug_ast)
        debugAST(root);

    IRGen irGen(root, parser.allNamespaces);
    std::vector<IRChunk*> irChunks = irGen.generateIR();

    for (auto& stmt : root)
        delete stmt;
    root.clear();

    if (!irGen.cont)
        return;

    if (buildMode.debug_ir)
    {
        for (auto& irc : irChunks)
        {
            std::cout << irc->name << ":\n";
            debugInstructions(irc);
            std::cout << std::endl;
        }
    }

    CodeGen codegen(parser.allNamespaces);
    for (auto& irc : irChunks)
        codegen.generateCode(irc);

    if (buildMode.debug_code)
    {
        for (auto& irc : irChunks)
        {
            std::cout << irc->name << "@" << irc->chunk << ":\n";
            dissambleChunk(irc->chunk);
            std::cout << std::endl;
        }
    }

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