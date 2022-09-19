//
// Created by cembelentepe on 09/09/22.
//

#pragma once

#include <utility>

#include "Value.h"
#include "AstVisitor.h"
#include "Environment.h"

class Interpreter : public ExprVisitor<Value>, public StmtVisitor<void>
{
public:
	explicit Interpreter(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os);
	~Interpreter() override;

	void run();
	[[nodiscard]] bool fail() const;

	void visit(StmtDeclVar* stmt) override;
	void visit(StmtDeclFunc* stmt) override;

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
	void visit(ExprAddrOf* expr) override;
	void visit(ExprDeref* expr) override;
	void visit(ExprNew* expr) override;

	Value runFunction(StmtDeclFunc* func, std::vector<Value> args);

private:
	using EnvType = Environment<std::shared_ptr<Value>>;
	using BinaryFuncDef = std::tuple<TokenType, PrimitiveTag, PrimitiveTag>;
	using BinaryFuncDec = auto (*)(Value::Data, Value::Data) -> Value::Data;
	using UnaryFuncDef = std::tuple<TokenType, PrimitiveTag>;
	using UnaryFuncDec = auto (*)(Value::Data) -> Value::Data;

private:
	std::vector<std::unique_ptr<Stmt>>& root;
	std::ostream& os;
	std::unique_ptr<EnvType> environment;
	bool failed;

	static const std::vector<std::tuple<BinaryFuncDef, BinaryFuncDec>> binaryOps;
	static const std::vector<std::tuple<UnaryFuncDef, UnaryFuncDec>> unaryOps;

	class ret_value : std::exception
	{
	public:
		Value val;

		explicit ret_value(Value val)
			: val(std::move(val))
		{
		}
	};
};
