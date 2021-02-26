#include "DeclParser.hpp"
#include "Parser.hpp"

#include "Native/Native.hpp"
#include <iostream>

DeclParser::DeclParser(std::vector<std::unique_ptr<Stmt>>& root)
    : global(std::make_unique<Namespace<Variable>>()), root(root), currentNamespace(global.get()), hadError(false)
{
    NativeFunc::InitTo(this);
}

std::unique_ptr<Namespace<Variable>> DeclParser::parse()
{
    for (auto& decl : root)
    {
        try
        {
            decl->accept(this);
        }
        catch (const Parser::TokenError& err)
        {
            std::cout << err << std::endl;
            hadError = true;
        }
    }

    return std::move(global);
}

bool DeclParser::fail()
{
    return hadError;
}

void DeclParser::addNativeCallable(NativeFunc* func)
{
    currentNamespace = global.get();
    for(auto& str : func->getNamespace())
    {
        currentNamespace = currentNamespace->makeNamespace(str);
    }
    currentNamespace->addVariable(Variable(func->tokenName(), func->getType(), true));
    currentNamespace = global.get();
}

void DeclParser::visit(DeclVar* decl)
{
    currentNamespace->addVariable(Variable(decl->name, decl->type, decl->initter != nullptr));
}

void DeclParser::visit(DeclFunc* decl)
{
    decl->address = currentNamespace->getAddress();
    currentNamespace->addVariable(Variable(decl->name, decl->type, true));
}

void DeclParser::visit(DeclNamespace* decl)
{
    currentNamespace = currentNamespace->makeNamespace(decl->name);
    for (auto& stmt : decl->body)
        stmt->accept(this);
    currentNamespace = currentNamespace->parent;
}

void DeclParser::visit(StmtBody* stmt)
{
}

void DeclParser::visit(StmtExpr* stmt)
{
}

void DeclParser::visit(StmtReturn* stmt)
{
}

void DeclParser::visit(StmtIf* stmt)
{
}

void DeclParser::visit(StmtWhile* stmt)
{
}