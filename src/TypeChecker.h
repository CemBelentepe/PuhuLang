//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include "AstVisitor.h"

class TypeChecker : public ExprVisitor<std::shared_ptr<Type>>, public StmtVisitor<void>
{
public:
	explicit TypeChecker(std::vector<std::unique_ptr<Stmt>>& root);
	~TypeChecker() override;

	void check();

	void visit(StmtExpr* stmt) override;

	void visit(ExprBinary* expr) override;
	void visit(ExprUnary* expr) override;
	void visit(ExprLiteral* expr) override;

private:
	std::vector<std::unique_ptr<Stmt>>& root;
};
