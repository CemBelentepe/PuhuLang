//
// Created by cembelentepe on 08/09/22.
//

#include "AstDebugger.h"
#include <iostream>

AstDebugger::AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os)
	: root(root), os(os), isShowTypes(false), indent(0)
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

void AstDebugger::visit(StmtDeclVar* stmt)
{
	os << indented() << "DECL " << stmt->name.lexeme << ": " << stmt->type->toString();
	if(stmt->init)
		os << " = " << stmt->init->accept(this);
	os << "\n";
}

void AstDebugger::visit(StmtExpr* stmt)
{
	os << indented() << "EXPR: " << stmt->expr->accept(this) << "\n";
}

void AstDebugger::visit(StmtBlock* stmt)
{
	os << indented() << "BEGIN\n";
	indent++;
	for(auto& s : stmt->stmts)
		s->accept(this);
	indent--;
	os << indented() << "END\n";
}

void AstDebugger::visit(StmtIf* stmt)
{
	os << indented() << "IF (" << stmt->cond->accept(this) << ")\n";
	indent++;
	stmt->then->accept(this);
	indent--;
	if(stmt->els)
	{
		os << indented() << "ELSE\n";
		indent++;
		stmt->els->accept(this);
		indent--;
	}
	os << indented() << "END IF\n";
}

void AstDebugger::visit(StmtWhile* stmt)
{
	os << indented() << "WHILE (" << stmt->cond->accept(this) << ")\n";
	indent++;
	stmt->body->accept(this);
	indent--;
	os << indented() << "END WHILE\n";
}

void AstDebugger::visit(StmtFor* stmt)
{
	os << indented() << (stmt->init ? stmt->init->accept(this) + "\n": "");
	os << indented() << "FOR (" << (stmt->cond ? stmt->cond->accept(this) : "true") << ")\n";
	indent++;
	stmt->body->accept(this);
	if (stmt->fin) os << indented() << stmt->fin->accept(this) << "\n";
	indent--;
	os << indented() << "END FOR";
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

void AstDebugger::visit(ExprVarGet* expr)
{
	this->result = "(GET " + expr->name.getLexeme() + ")";
}

void AstDebugger::visit(ExprVarSet* expr)
{
	this->result = "(SET " + expr->name.getLexeme() + ", " + expr->val->accept(this) + ")";
}


std::string AstDebugger::indented() const
{
	return std::string(indent, '\t');
}
