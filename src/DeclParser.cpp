#include "DeclParser.hpp"
#include "Parser.hpp"

#include <iostream>

DeclParser::DeclParser(std::vector<std::unique_ptr<Stmt>>& root)
    : global(std::make_unique<Namespace<Variable>>()), root(root), currentNamespace(global.get()), hadError(false)
{
}

std::unique_ptr<Namespace<Variable>> DeclParser::parse()
{
    for(auto& decl : root)
    {
        try
        {
            decl->accept(this);
        }
        catch(const Parser::TokenError& err)
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

void DeclParser::visit(StmtExpr* stmt)
{
}

void DeclParser::visit(DeclVar* decl)
{
    currentNamespace->addVariable(Variable(decl->name, decl->type, decl->initter != nullptr));
}

void DeclParser::visit(DeclFunc* decl)
{
    currentNamespace->addVariable(Variable(decl->name, decl->type, true));
}

void DeclParser::visit(StmtBody* stmt) 
{
    
}
