//
// Created by cembelentepe on 08/09/22.
//

#pragma once
#include "AstVisitor.h"
#include <string>

class AstDebugger: public StmtVisitor<void>, public ExprVisitor<std::string>
{
public:
	explicit AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os);
	~AstDebugger() override;

	void debug();
	void setShowTypes(bool isShow);

	void visit(StmtExpr* stmt) override;

	void visit(ExprBinary* expr) override;
	void visit(ExprUnary* expr) override;
	void visit(ExprLiteral* expr) override;

private:
	std::vector<std::unique_ptr<Stmt>>& root;
	std::ostream& os;
	bool isShowTypes;
};
