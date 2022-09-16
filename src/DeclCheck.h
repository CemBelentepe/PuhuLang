//
// Created by cembelentepe on 16/09/22.
//

#pragma once

#include <unordered_map>
#include "AstVisitor.h"

class DeclCheck : public StmtVisitor<void>
{
public:
	explicit DeclCheck(std::vector<std::unique_ptr<Stmt>>& root);
	~DeclCheck() override;

	std::unordered_map<std::string, Stmt*> check();
	[[nodiscard]] bool fail() const;

	void visit(StmtDeclVar* stmt) override;
	void visit(StmtDeclFunc* stmt) override;

	void visit(StmtExpr* stmt) override;
	void visit(StmtBlock* stmt) override;
	void visit(StmtIf* stmt) override;
	void visit(StmtWhile* stmt) override;
	void visit(StmtFor* stmt) override;
	void visit(StmtReturn* stmt) override;

private:
	static void redefinitionError(const Token& name);
	static void invalidDeclarationError(const Token& tok);

private:
	std::vector<std::unique_ptr<Stmt>>& root;
	std::unordered_map<std::string, Stmt*> decls;
	bool failed;
};
