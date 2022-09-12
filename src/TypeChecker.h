//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include "AstVisitor.h"
#include "Environment.h"
#include <unordered_map>

class TypeChecker : public ExprVisitor<TypePtr>, public StmtVisitor<void>
{
public:
	explicit TypeChecker(std::vector<std::unique_ptr<Stmt>>& root);
	~TypeChecker() override;

	void check();
	[[nodiscard]] bool fail() const;

	void visit(StmtDeclVar* stmt) override;

	void visit(StmtExpr* stmt) override;
	void visit(StmtBlock* stmt) override;
	void visit(StmtIf* stmt) override;
	void visit(StmtWhile* stmt) override;
	void visit(StmtFor* stmt) override;
	void visit(StmtReturn* stmt) override;

	void visit(ExprBinary* expr) override;
	void visit(ExprUnary* expr) override;
	void visit(ExprLiteral* expr) override;
	void visit(ExprVarGet* expr) override;
	void visit(ExprVarSet* expr) override;
	void visit(ExprCall* expr) override;

private:
	using BinaryFuncDef = std::tuple<TokenType, PrimitiveTag, PrimitiveTag>;
	using UnaryFuncDef = std::tuple<TokenType, PrimitiveTag>;

private:
	std::vector<std::unique_ptr<Stmt>>& root;
	std::unique_ptr<Environment<TypePtr>> environment;
	bool failed;
	static const std::vector<std::tuple<BinaryFuncDef, PrimitiveTag>> binaryOperations;
	static const std::vector<std::tuple<UnaryFuncDef, PrimitiveTag>> unaryOps;
};
