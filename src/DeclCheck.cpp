//
// Created by cembelentepe on 16/09/22.
//

#include <stdexcept>
#include <iostream>
#include <sstream>
#include "DeclCheck.h"

DeclCheck::DeclCheck(std::vector<std::unique_ptr<Stmt>>& root)
	: root(root), failed(false)
{

}

DeclCheck::~DeclCheck() = default;

std::unordered_map<std::string, TypePtr> DeclCheck::check()
{
	for (auto& stmt : root)
	{
		try
		{
			stmt->accept(this);
		}
		catch (std::runtime_error& e)
		{
			failed = true;
			std::cerr << e.what() << std::endl;
		}
	}

	return std::move(decls);
}

bool DeclCheck::fail() const
{
	return failed;
}

void DeclCheck::visit(StmtDeclVar* stmt)
{
}

void DeclCheck::visit(StmtDeclFunc* stmt)
{
	std::string name = stmt->name.getLexeme();
	if (decls.find(name) == decls.end())
		decls.insert({ name, stmt->type });
	else
		redefinitionError(stmt->name);
}

void DeclCheck::visit(StmtExpr* stmt)
{
	invalidDeclarationError(stmt->semicolon);
}

void DeclCheck::visit(StmtBlock* stmt)
{
	invalidDeclarationError(stmt->openBrace);
}

void DeclCheck::visit(StmtIf* stmt)
{
	invalidDeclarationError(stmt->ifTok);
}

void DeclCheck::visit(StmtWhile* stmt)
{
	invalidDeclarationError(stmt->whileTok);
}

void DeclCheck::visit(StmtFor* stmt)
{
	invalidDeclarationError(stmt->forTok);
}

void DeclCheck::visit(StmtReturn* stmt)
{
	invalidDeclarationError(stmt->ret);
}

void DeclCheck::redefinitionError(const Token& name)
{
	std::stringstream ssErr;
	ssErr << "[ERROR " << name.line << ":" << name.col << "] Redefinition of `" << name.getLexeme() << "`.";
	throw std::runtime_error(ssErr.str());
}

void DeclCheck::invalidDeclarationError(const Token& tok)
{
	std::stringstream ssErr;
	ssErr << "[ERROR " << tok.line << ":" << tok.col << "] Statement starting with `" << tok.getLexeme()
		  << "` cannot be a declaration.";
	throw std::runtime_error(ssErr.str());
}

