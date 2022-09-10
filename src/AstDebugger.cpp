//
// Created by cembelentepe on 08/09/22.
//

#include "AstDebugger.h"
#include <iostream>

AstDebugger::AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os)
	: root(root), os(os), isShowTypes(false)
{

}

AstDebugger::~AstDebugger() = default;

void AstDebugger::debug()
{
	for (auto& stmt : root)
	{
		stmt->accept(this);
	}
	os << std::endl;
}

void AstDebugger::setShowTypes(bool isShow)
{
	isShowTypes = isShow;
}

void AstDebugger::visit(StmtExpr* stmt)
{
	os << "EXPR: " << stmt->expr->accept(this) << "\n";
}

void AstDebugger::visit(StmtBlock* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void AstDebugger::visit(StmtIf* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void AstDebugger::visit(StmtWhile* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void AstDebugger::visit(StmtFor* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void AstDebugger::visit(StmtReturn* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void AstDebugger::visit(ExprBinary* expr)
{
	std::string lhs = expr->lhs->accept(this);
	std::string rhs = expr->rhs->accept(this);
	this->result = "(" + std::string(expr->op.lexeme) + " " + lhs + ", " + rhs + ")";
	if (isShowTypes)
		this->result += ": " + expr->type->toString();
}

void AstDebugger::visit(ExprUnary* expr)
{
	std::string rhs = expr->rhs->accept(this);
	this->result = "(" + std::string(expr->op.lexeme) + " " + rhs + ")";
	if (isShowTypes)
		this->result += ": " + expr->type->toString();
}
void AstDebugger::visit(ExprLiteral* expr)
{
	this->result = "(" + std::string(expr->literal.lexeme) + ")";
	if (isShowTypes)
		this->result += ": " + expr->type->toString();
}
