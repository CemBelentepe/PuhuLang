//
// Created by cembelentepe on 09/09/22.
//

#pragma once

#include "Value.h"
#include "AstVisitor.h"

class Interpreter : public ExprVisitor<Value>, public StmtVisitor<void>
{
public:
	explicit Interpreter(std::vector<std::shared_ptr<Stmt>>& root, std::ostream& os);
	~Interpreter() override;

	void run();

	void visit(StmtExpr* stmt) override;

	void visit(ExprBinary* expr) override;
	void visit(ExprUnary* expr) override;
	void visit(ExprLiteral* expr) override;
private:



private:
	std::vector<std::shared_ptr<Stmt>>& root;
	std::ostream& os;
};
