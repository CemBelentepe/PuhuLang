//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <set>
#include <unordered_map>
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
	using BinaryFuncDef = std::tuple<TokenType, PrimitiveTag, PrimitiveTag>;
	using UnaryFuncDef = std::tuple<TokenType, PrimitiveTag>;

private:
	std::vector<std::unique_ptr<Stmt>>& root;
	static std::vector<std::tuple<BinaryFuncDef, PrimitiveTag>> binaryOperations;
	static std::vector<std::tuple<UnaryFuncDef, PrimitiveTag>> unaryOps;
};
