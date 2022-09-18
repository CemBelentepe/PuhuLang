//
// Created by cembelentepe on 09/09/22.
//

#include "Interpreter.h"
#include "Callable.h"
#include <iostream>
#include <algorithm>

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os)
	: root(root), os(os), failed(false), environment(std::make_unique<Environment<Value>>(nullptr))
{
	auto nativeTypes = FunctionNative::getNativeTypes();
	for (auto& native : FunctionNative::getNativeFuncs())
		this->environment->addVariable(native.first,
			Value(std::make_shared<FunctionNative>(native.second), nativeTypes[native.first]));
}

Interpreter::~Interpreter() = default;

void Interpreter::run()
{
	try
	{
		for (auto& stmt : root)
		{
			stmt->accept(this);
		}
	}
	catch (std::runtime_error& e)
	{
		failed = true;
		std::cerr << e.what() << std::endl;
	}

	try
	{
		Value mainFunc;
		try
		{
			mainFunc = environment->getVariable("main");
		}
		catch (std::runtime_error& e)
		{
			failed = true;
			std::cerr << "[ERROR] Entry point `main` is not defined on the program." << std::endl;
		}

		auto mainType = TypeFactory::getFunction(TypeFactory::getPrimitive(PrimitiveTag::VOID), {});
		if (!mainFunc.getType()->isSame(mainType))
		{
			failed = true;
			std::cerr << "[ERROR] Main function should have a type of `" << mainType->toString() << "` but `"
					  << mainFunc.getType()->toString() << "` is provided.";
		}

		mainFunc.getDataTyped<std::shared_ptr<Callable>>()->call(this, {});
	}
	catch (std::runtime_error& e)
	{
		failed = true;
		std::cerr << e.what() << std::endl;
	}
}

bool Interpreter::fail() const
{
	return failed;
}

void Interpreter::visit(StmtDeclVar* stmt)
{
	Value initVal;
	if (stmt->init)
		initVal = stmt->init->accept(this);

	environment->addVariable(stmt->name, initVal);
}

void Interpreter::visit(StmtDeclFunc* stmt)
{
	auto callable = Value(std::make_shared<FunctionUser>(stmt), stmt->type);
	environment->addVariable(stmt->name, callable);
}

void Interpreter::visit(StmtExpr* stmt)
{
	Value res = stmt->expr->accept(this);
}

void Interpreter::visit(StmtBlock* stmt)
{
	environment = std::make_unique<Environment<Value>>(std::move(environment));
	for (auto& s : stmt->stmts)
		s->accept(this);
	environment = environment->moveParent();
}

void Interpreter::visit(StmtIf* stmt)
{
	Value cond = stmt->cond->accept(this);
	if (cond.getDataTyped<bool>())
		stmt->then->accept(this);
	else if (stmt->els)
		stmt->els->accept(this);
}

void Interpreter::visit(StmtWhile* stmt)
{
	while (stmt->cond->accept(this).getDataTyped<bool>())
		stmt->body->accept(this);
}

void Interpreter::visit(StmtFor* stmt)
{
	if (stmt->init) stmt->init->accept(this);

	while (stmt->cond == nullptr || stmt->cond->accept(this).getDataTyped<bool>())
	{
		stmt->body->accept(this);
		if (stmt->fin) stmt->fin->accept(this);
	}
}

void Interpreter::visit(StmtReturn* stmt)
{
	throw ret_value(stmt->expr->accept(this));
}

void Interpreter::visit(ExprBinary* expr)
{
	Value lhs = expr->lhs->accept(this);

	if (expr->op.type == TokenType::OR)
	{
		if (lhs.getDataTyped<bool>())
			this->result = lhs;
		else
			this->result = expr->rhs->accept(this);
		return;
	}
	else if (expr->op.type == TokenType::AND)
	{
		if (!lhs.getDataTyped<bool>())
			this->result = lhs;
		else
			this->result = expr->rhs->accept(this);
		return;
	}

	Value rhs = expr->rhs->accept(this);

	using PT = PrimitiveTag;
	PT typeLhsP = ((TypePrimitive*)(lhs.getType().get()))->special_tag;
	PT typeRhsP = ((TypePrimitive*)(rhs.getType().get()))->special_tag;

	auto it = std::find_if(binaryOps.begin(), binaryOps.end(), [&](auto val)
	{
	  auto first = std::get<0>(val);
	  return std::get<0>(first) == expr->op.type &&
		  std::get<1>(first) == typeLhsP &&
		  std::get<2>(first) == typeRhsP;
	});

	if (it == binaryOps.end())
		throw std::runtime_error("[DEV] Not implemented binary operation for the interpreter.");

	auto ret = std::get<1>(*it)(lhs.getData(), rhs.getData());
	this->result = Value(ret, expr->type);
}

void Interpreter::visit(ExprUnary* expr)
{
	Value rhs = expr->rhs->accept(this);

	PrimitiveTag typeRhsP = ((TypePrimitive*)(rhs.getType().get()))->special_tag;

	auto it = std::find_if(unaryOps.begin(), unaryOps.end(), [&](auto val)
	{
	  auto first = std::get<0>(val);
	  return std::get<0>(first) == expr->op.type &&
		  std::get<1>(first) == typeRhsP;
	});

	if (it == unaryOps.end())
		throw std::runtime_error("[DEV] Not implemented unary operation for the interpreter.");

	auto ret = std::get<1>(*it)(rhs.getData());
	this->result = Value(ret, expr->type);
}

void Interpreter::visit(ExprLiteral* expr)
{
	this->result = expr->literal.getValue();
}

void Interpreter::visit(ExprVarGet* expr)
{
	this->result = environment->getVariable(expr->name);
}

void Interpreter::visit(ExprVarSet* expr)
{
	Value val = expr->val->accept(this);
	this->environment->setVariable(expr->name, val);
	this->result = val;
}

void Interpreter::visit(ExprCall* expr)
{
	Value funcVal = expr->callee->accept(this);
	std::vector<Value> args;
	std::transform(expr->args.begin(), expr->args.end(), std::back_inserter(args), [this](auto& arg)
	{
	  return arg->accept(this);
	});

	this->result = funcVal.getDataTyped<std::shared_ptr<Callable>>()->call(this, args);
}

void Interpreter::visit(ExprAddrOf* expr)
{
	throw NotImplementedException();
}

void Interpreter::visit(ExprDeref* expr)
{
	throw NotImplementedException();
}

Value Interpreter::runFunction(StmtDeclFunc* func, std::vector<Value> args)
{
	environment = std::make_unique<Environment<Value>>(std::move(environment));

	if (func->params.size() != args.size())
		throw std::runtime_error("[DEV] Type check failed to check the number of args and params in a call.");

	for (size_t i = 0; i < func->params.size(); i++)
		environment->addVariable(std::get<1>(func->params[i]), args[i]);

	Value retVal = Value::getVoid();
	try
	{
		func->body->accept(this);
	}
	catch (ret_value& e)
	{
		retVal = e.val;
	}

	environment = environment->moveParent();

	return retVal;
}

const std::vector<std::tuple<Interpreter::UnaryFuncDef, Interpreter::UnaryFuncDec>> Interpreter::unaryOps = {
	{{ TokenType::MINUS, PrimitiveTag::INT }, [](Value::Data rhs)
	{ return Value::Data(-std::get<int>(rhs)); }},
	{{ TokenType::MINUS, PrimitiveTag::FLOAT }, [](Value::Data rhs)
	{ return Value::Data(-std::get<float>(rhs)); }},
	{{ TokenType::MINUS, PrimitiveTag::DOUBLE }, [](Value::Data rhs)
	{ return Value::Data(-std::get<double>(rhs)); }},
	{{ TokenType::PLUS, PrimitiveTag::INT }, [](Value::Data rhs)
	{ return Value::Data(+std::get<int>(rhs)); }},
	{{ TokenType::PLUS, PrimitiveTag::FLOAT }, [](Value::Data rhs)
	{ return Value::Data(+std::get<float>(rhs)); }},
	{{ TokenType::PLUS, PrimitiveTag::DOUBLE }, [](Value::Data rhs)
	{ return Value::Data(+std::get<double>(rhs)); }},
	{{ TokenType::PLUS_PLUS, PrimitiveTag::INT }, [](Value::Data rhs)
	{ return Value::Data(++std::get<int>(rhs)); }},
	{{ TokenType::MINUS_MINUS, PrimitiveTag::INT }, [](Value::Data rhs)
	{ return Value::Data(--std::get<int>(rhs)); }},
	{{ TokenType::BANG, PrimitiveTag::BOOL }, [](Value::Data rhs)
	{ return Value::Data(!std::get<bool>(rhs)); }},
	{{ TokenType::TILDE, PrimitiveTag::INT }, [](Value::Data rhs)
	{ return Value::Data(~std::get<int>(rhs)); }}};
const std::vector<std::tuple<Interpreter::BinaryFuncDef, Interpreter::BinaryFuncDec>>Interpreter::binaryOps = {
	{{ TokenType::OR, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<bool>(lhs) ||
			  std::get<bool>(rhs));
	}},
	{{ TokenType::AND, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<bool>(lhs) &&
			  std::get<bool>(rhs));
	}},
	{{ TokenType::BIT_OR, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) |
			  std::get<int>(rhs));
	}},
	{{ TokenType::BIT_XOR, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) ^
			  std::get<int>(rhs));
	}},
	{{ TokenType::BIT_AND, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) &
			  std::get<int>(rhs));
	}},
	{{ TokenType::BITSHIFT_LEFT, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs)
			  << std::get<int>(
				  rhs));
	}},
	{{ TokenType::BITSHIFT_RIGHT, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs)
			  >> std::get<int>(
				  rhs));
	}},
	{{ TokenType::MODULUS, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) %
			  std::get<int>(rhs));
	}},
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<bool>(lhs) ==
			  std::get<bool>(rhs));
	}},
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) ==
			  std::get<char>(rhs));
	}},
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) ==
			  std::get<int>(rhs));
	}},
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) ==
			  std::get<float>(rhs));
	}},
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) ==
			  std::get<double>(rhs));
	}},
	{{ TokenType::BANG_EQUAL, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<bool>(lhs) !=
			  std::get<bool>(rhs));
	}},
	{{ TokenType::BANG_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) !=
			  std::get<char>(rhs));
	}},
	{{ TokenType::BANG_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) !=
			  std::get<int>(rhs));
	}},
	{{ TokenType::BANG_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) !=
			  std::get<float>(rhs));
	}},
	{{ TokenType::BANG_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) !=
			  std::get<double>(rhs));
	}},
	{{ TokenType::LESS, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) <
			  std::get<char>(rhs));
	}},
	{{ TokenType::LESS, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) <
			  std::get<int>(rhs));
	}},
	{{ TokenType::LESS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) <
			  std::get<float>(rhs));
	}},
	{{ TokenType::LESS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) <
			  std::get<double>(rhs));
	}},
	{{ TokenType::LESS_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) <=
			  std::get<char>(rhs));
	}},
	{{ TokenType::LESS_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) <=
			  std::get<int>(rhs));
	}},
	{{ TokenType::LESS_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) <=
			  std::get<float>(rhs));
	}},
	{{ TokenType::LESS_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) <=
			  std::get<double>(rhs));
	}},
	{{ TokenType::GREAT, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) >
			  std::get<char>(rhs));
	}},
	{{ TokenType::GREAT, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) >
			  std::get<int>(rhs));
	}},
	{{ TokenType::GREAT, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) >
			  std::get<float>(rhs));
	}},
	{{ TokenType::GREAT, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) >
			  std::get<double>(rhs));
	}},
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<char>(lhs) >=
			  std::get<char>(rhs));
	}},
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) >=
			  std::get<int>(rhs));
	}},
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) >=
			  std::get<float>(rhs));
	}},
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) >=
			  std::get<double>(rhs));
	}},
	{{ TokenType::PLUS, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) +
			  std::get<int>(rhs));
	}},
	{{ TokenType::PLUS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) +
			  std::get<float>(rhs));
	}},
	{{ TokenType::PLUS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) +
			  std::get<double>(rhs));
	}},
	{{ TokenType::MINUS, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) -
			  std::get<int>(rhs));
	}},
	{{ TokenType::MINUS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) -
			  std::get<float>(rhs));
	}},
	{{ TokenType::MINUS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) -
			  std::get<double>(rhs));
	}},
	{{ TokenType::STAR, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) *
			  std::get<int>(rhs));
	}},
	{{ TokenType::STAR, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) *
			  std::get<float>(rhs));
	}},
	{{ TokenType::STAR, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) *
			  std::get<double>(rhs));
	}},
	{{ TokenType::SLASH, PrimitiveTag::INT, PrimitiveTag::INT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<int>(lhs) /
			  std::get<int>(rhs));
	}},
	{{ TokenType::SLASH, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<float>(lhs) /
			  std::get<float>(rhs));
	}},
	{{ TokenType::SLASH, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE }, [](Value::Data lhs, Value::Data rhs)
	{
	  return Value::Data(
		  std::get<double>(lhs) /
			  std::get<double>(rhs));
	}}};
