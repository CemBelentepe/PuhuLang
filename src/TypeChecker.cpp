//
// Created by cembelentepe on 08/09/22.
//

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "TypeChecker.h"

TypeChecker::TypeChecker(std::vector<std::unique_ptr<Stmt>>& root)
	: root(root), failed(false)
{
}

TypeChecker::~TypeChecker() = default;

void TypeChecker::check()
{
	for (auto& stmt : root)
	{
		try
		{
			stmt->accept(this);
		}
		catch (std::runtime_error& e)
		{
			failed = true;
			std::cerr << e.what() << std::endl;
		}
	}
}

bool TypeChecker::fail() const
{
	return failed;
}

void TypeChecker::visit(StmtDeclVar* stmt)
{
	environment->addVariable(stmt->name, stmt->type);
	if (stmt->init)
	{
		TypePtr initType = stmt->init->accept(this);
		if (!initType->isSame(stmt->type))
		{
			std::stringstream ssErr;

			ssErr << "[ERROR " << stmt->eq.line << ":" << stmt->eq.col << "]  Cannot initialize type of `"
				  << stmt->type->toString()
				  << "` with the type of `" << initType->toString() << "`.";

			throw std::runtime_error(ssErr.str());
		}
	}
}

void TypeChecker::visit(StmtExpr* stmt)
{
	stmt->expr->accept(this);
}

void TypeChecker::visit(StmtBlock* stmt)
{
	environment = std::make_unique<Environment<TypePtr>>(std::move(environment));
	for (auto& s : stmt->stmts)
	{
		try
		{
			s->accept(this);
		}
		catch (std::runtime_error& e)
		{
			failed = true;
			std::cerr << e.what() << std::endl;
		}
	}
	environment = std::move(environment->moveParent());
}

void TypeChecker::visit(StmtIf* stmt)
{
	stmt->cond->accept(this);
	stmt->then->accept(this);
	if (stmt->els) stmt->els->accept(this);

	if (!stmt->cond->type->isSame(TypeFactory::getPrimitive(PrimitiveTag::BOOL)))
		throw std::runtime_error("Condition of the `if` statement must evaluate to a `bool` type.");
}

void TypeChecker::visit(StmtWhile* stmt)
{
	stmt->cond->accept(this);
	stmt->body->accept(this);

	if (!stmt->cond->type->isSame(TypeFactory::getPrimitive(PrimitiveTag::BOOL)))
		throw std::runtime_error("Condition of the `while` statement must evaluate to a `bool` type.");
}

void TypeChecker::visit(StmtFor* stmt)
{
	if (stmt->init) stmt->init->accept(this);
	if (stmt->cond) stmt->cond->accept(this);
	if (stmt->fin) stmt->fin->accept(this);
	stmt->body->accept(this);

	if (stmt->cond && !stmt->cond->type->isSame(TypeFactory::getPrimitive(PrimitiveTag::BOOL)))
		throw std::runtime_error("Condition of the `while` statement must evaluate to a `bool` type.");
}

void TypeChecker::visit(StmtReturn* stmt)
{
	throw std::runtime_error("Not implemented.");
}

void TypeChecker::visit(ExprBinary* expr)
{
	auto typeLhs = expr->lhs->accept(this);
	auto typeRhs = expr->rhs->accept(this);

	std::shared_ptr<Type> resType = TypeFactory::getNull();

	// TODO assuming everything is primitive
	if (typeLhs->tag == Type::Tag::PRIMITIVE && typeRhs->tag == Type::Tag::PRIMITIVE)
	{
		using PT = PrimitiveTag;
		PT typeLhsP = ((TypePrimitive*)(typeLhs.get()))->special_tag;
		PT typeRhsP = ((TypePrimitive*)(typeRhs.get()))->special_tag;

		auto it = std::find_if(binaryOperations.begin(), binaryOperations.end(), [&](auto x)
		{
		  auto first = std::get<0>(x);
		  return std::get<0>(first) == expr->op.type &&
			  std::get<1>(first) == typeLhsP &&
			  std::get<2>(first) == typeRhsP;
		});

		if (it != binaryOperations.end())
			resType = TypeFactory::getPrimitive(std::get<1>(*it));
	}

	expr->type = resType;

	if (resType->tag == Type::Tag::ERROR)
	{
		std::stringstream ssErr;

		ssErr << "[ERROR " << expr->op.line << ":" << expr->op.col << "] Operands of `" << expr->op.lexeme
			  << "` cannot be the types of `" << typeLhs->toString() << "` and `" << typeRhs->toString() << "`.";

		throw std::runtime_error(ssErr.str());
	}

	this->result = expr->type;
}

void TypeChecker::visit(ExprUnary* expr)
{
	auto typeRhs = expr->rhs->accept(this);

	auto resType = TypeFactory::getNull();

	// TODO assuming everything is primitive
	if (typeRhs->tag == Type::Tag::PRIMITIVE)
	{
		PrimitiveTag typeRhsP = ((TypePrimitive*)(typeRhs.get()))->special_tag;

		auto it = std::find_if(unaryOps.begin(), unaryOps.end(), [&](auto x)
		{
		  auto first = std::get<0>(x);
		  return std::get<0>(first) == expr->op.type &&
			  std::get<1>(first) == typeRhsP;
		});

		if (it != unaryOps.end())
			resType = TypeFactory::getPrimitive(std::get<1>(*it));
	}

	expr->type = resType;

	if (resType->tag == Type::Tag::ERROR)
	{
		std::stringstream ssErr;

		ssErr << "[ERROR " << expr->op.line << ":" << expr->op.col << "] Operands of `" << expr->op.lexeme
			  << "` cannot be the type of `" << typeRhs->toString() << "`.";

		throw std::runtime_error(ssErr.str());
	}

	this->result = expr->type;
}

void TypeChecker::visit(ExprLiteral* expr)
{
	expr->type = expr->literal.getType();
	this->result = expr->type;
}

void TypeChecker::visit(ExprVarGet* expr)
{
	expr->type = environment->getVariable(expr->name);
	this->result = expr->type;
}

void TypeChecker::visit(ExprVarSet* expr)
{
	expr->type = environment->getVariable(expr->name);
	TypePtr valType = expr->val->accept(this);
	if (expr->type->isConst)
	{
		std::stringstream ssErr;

		ssErr << "[ERROR " << expr->name.line << ":" << expr->name.col << "] Cannot assign a to variable `" << expr->name.lexeme
			  << "` since it is const.";

		throw std::runtime_error(ssErr.str());
	}
	if (!valType->isSame(expr->type))
	{
		std::stringstream ssErr;

		ssErr << "[ERROR " << expr->op.line << ":" << expr->op.col << "] Assigning to `" << expr->type->toString()
			  << "` from `" << valType->toString() << "`.";

		throw std::runtime_error(ssErr.str());
	}

	this->result = expr->type;
}

const std::vector<std::tuple<TypeChecker::UnaryFuncDef, PrimitiveTag>> TypeChecker::unaryOps = {
	{{ TokenType::MINUS, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::MINUS, PrimitiveTag::FLOAT }, PrimitiveTag::FLOAT },
	{{ TokenType::MINUS, PrimitiveTag::DOUBLE }, PrimitiveTag::DOUBLE },
	{{ TokenType::PLUS, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::PLUS, PrimitiveTag::FLOAT }, PrimitiveTag::FLOAT },
	{{ TokenType::PLUS, PrimitiveTag::DOUBLE }, PrimitiveTag::DOUBLE },
	{{ TokenType::PLUS_PLUS, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::MINUS_MINUS, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::BANG, PrimitiveTag::BOOL }, PrimitiveTag::BOOL },
	{{ TokenType::TILDE, PrimitiveTag::INT }, PrimitiveTag::INT }};
const std::vector<std::tuple<TypeChecker::BinaryFuncDef, PrimitiveTag>>TypeChecker::binaryOperations = {
	{{ TokenType::OR, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, PrimitiveTag::BOOL },
	{{ TokenType::AND, PrimitiveTag::BOOL, PrimitiveTag::BOOL }, PrimitiveTag::BOOL },
	{{ TokenType::BIT_OR, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::BIT_XOR, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::BIT_AND, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::BITSHIFT_LEFT, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::INT },
	{{ TokenType::BITSHIFT_RIGHT, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::INT },
	{{ TokenType::MODULUS, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::BOOL, PrimitiveTag::BOOL },
	 PrimitiveTag::BOOL },
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR },
	 PrimitiveTag::BOOL },
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::BOOL },
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::EQUAL_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::BANG_EQUAL, PrimitiveTag::BOOL, PrimitiveTag::BOOL },
	 PrimitiveTag::BOOL },
	{{ TokenType::BANG_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR },
	 PrimitiveTag::BOOL },
	{{ TokenType::BANG_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::BOOL },
	{{ TokenType::BANG_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::BANG_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS, PrimitiveTag::CHAR, PrimitiveTag::CHAR }, PrimitiveTag::BOOL },
	{{ TokenType::LESS, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::BOOL },
	{{ TokenType::LESS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::LESS_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT, PrimitiveTag::CHAR, PrimitiveTag::CHAR },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::BOOL },
	{{ TokenType::GREAT, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::CHAR, PrimitiveTag::CHAR },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::INT, PrimitiveTag::INT },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::BOOL },
	{{ TokenType::GREAT_EQUAL, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::BOOL },
	{{ TokenType::PLUS, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::PLUS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::FLOAT },
	{{ TokenType::PLUS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::DOUBLE },
	{{ TokenType::MINUS, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::MINUS, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::FLOAT },
	{{ TokenType::MINUS, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::DOUBLE },
	{{ TokenType::STAR, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::STAR, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::FLOAT },
	{{ TokenType::STAR, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::DOUBLE },
	{{ TokenType::SLASH, PrimitiveTag::INT, PrimitiveTag::INT }, PrimitiveTag::INT },
	{{ TokenType::SLASH, PrimitiveTag::FLOAT, PrimitiveTag::FLOAT },
	 PrimitiveTag::FLOAT },
	{{ TokenType::SLASH, PrimitiveTag::DOUBLE, PrimitiveTag::DOUBLE },
	 PrimitiveTag::DOUBLE }};
