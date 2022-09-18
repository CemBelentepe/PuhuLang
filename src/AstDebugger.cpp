//
// Created by cembelentepe on 08/09/22.
//

#include "AstDebugger.h"
#include <iostream>
#include <sstream>

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
	os << indented() << "VAR " << stmt->name.lexeme << ": " << stmt->type->toString();
	if(stmt->init)
		os << " = " << stmt->init->accept(this);
	os << "\n";
}

void AstDebugger::visit(StmtDeclFunc* stmt)
{
	os << indented() << "FUNC " << stmt->name.lexeme << ": " << stmt->type->toString() << "\n";
	stmt->body->accept(this);
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
	os << indented() << "FOR:\n";
	indent++;
	if(stmt->init)
		stmt->init->accept(this);
	os << indented() << "WHILE (" << (stmt->cond ? stmt->cond->accept(this) : "true") << ")\n";
	indent++;
	stmt->body->accept(this);
	if (stmt->fin) os << indented() << stmt->fin->accept(this) << "\n";
	indent--;
	os << indented() << "END\n";
	indent--;
	os << indented() << "END FOR\n";
}

void AstDebugger::visit(StmtReturn* stmt)
{
	os << indented() << "RETURN " << stmt->expr->accept(this) << "\n";
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

void AstDebugger::visit(ExprCall* expr)
{
	std::stringstream ss;
	ss << "(CALL " << expr->callee->accept(this) << " (";

	if(!expr->args.empty())
		ss << expr->args[0]->accept(this);
	for(size_t i = 1; i < expr->args.size(); i++)
		ss << ", " << expr->args[i]->accept(this);
	ss << "))";
	if(isShowTypes)
		ss << ": " << expr->type->toString();
	this->result = ss.str();
}

void AstDebugger::visit(ExprAddrOf* expr)
{
	std::stringstream ss;

	ss << "(ADDR " << expr->lvalue->accept(this) << ")";
	if(isShowTypes)
		ss << ": " << expr->type->toString();

	this->result = ss.str();
}

std::string AstDebugger::indented() const
{
	return std::string(indent, '\t');
}
