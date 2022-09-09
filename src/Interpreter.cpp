//
// Created by cembelentepe on 09/09/22.
//

#include <iostream>
#include "Interpreter.h"

Interpreter::Interpreter(std::vector<std::shared_ptr<Stmt>>& root, std::ostream& os)
	: root(root), os(os)
{
}

Interpreter::~Interpreter() = default;

void Interpreter::run()
{
	for (auto& stmt : root)
	{
		stmt->accept(this);
	}
}

void Interpreter::visit(StmtExpr* stmt)
{
	Value res = stmt->expr->accept(this);
	// TODO remove expr printing
	os << res.getInfo() << std::endl;
}

void Interpreter::visit(ExprBinary* expr)
{
	Value lhs = expr->lhs->accept(this);
	Value rhs = expr->rhs->accept(this);

	// TODO Implement
}

void Interpreter::visit(ExprUnary* expr)
{
	Value rhs = expr->rhs->accept(this);
	// TODO Implement
}

void Interpreter::visit(ExprLiteral* expr)
{
	this->result = expr->literal.getValue();
}


