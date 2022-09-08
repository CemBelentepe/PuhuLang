//
// Created by cembelentepe on 08/09/22.
//

#include <stdexcept>
#include "TypeChecker.h"
TypeChecker::TypeChecker(std::vector<std::unique_ptr<Stmt>>& root)
	: root(root)
{
}

TypeChecker::~TypeChecker() = default;

void TypeChecker::check()
{
	for(auto& stmt: root)
		stmt->accept(this);
}

void TypeChecker::visit(StmtExpr* stmt)
{
	stmt->expr->accept(this);
}

void TypeChecker::visit(ExprBinary* expr)
{
	auto typeLhs = expr->lhs->accept(this);
	auto typeRhs = expr->rhs->accept(this);

	std::shared_ptr<Type> resType = typeLhs;

	bool typeMatch = false;
	// TODO assuming everything is primitive
	if(typeLhs->tag == Type::Tag::PRIMITIVE)
	{
		using PT = PrimitiveTag;
		PT typeLhsP = ((TypePrimitive*)(typeLhs.get()))->special_tag;
		PT typeRhsP = ((TypePrimitive*)(typeRhs.get()))->special_tag;

		switch (expr->op.type)
		{
		case TokenType::OR:
		case TokenType::AND:
		{
			typeMatch = typeLhsP == typeRhsP && typeLhsP == PT::BOOL;
			break;
		}

		case TokenType::BIT_OR:
		case TokenType::BIT_XOR:
		case TokenType::BIT_AND:
		case TokenType::BITSHIFT_LEFT:
		case TokenType::BITSHIFT_RIGHT:
		{
			typeMatch = typeLhsP == typeRhsP && typeLhsP == PT::INT;
			break;
		}

		case TokenType::EQUAL_EQUAL:
		case TokenType::BANG_EQUAL:
		{
			typeMatch = typeLhsP == typeRhsP && typeLhsP >= PT::BOOL;
			resType = TypeFactory::getPrimitive(PrimitiveTag::BOOL);
			break;
		}

			// Comparison
		case TokenType::LESS:
		case TokenType::LESS_EQUAL:
		case TokenType::GREAT:
		case TokenType::GREAT_EQUAL:
		{
			typeMatch = typeLhsP == typeRhsP && typeLhsP >= PT::CHAR;
			resType = TypeFactory::getPrimitive(PrimitiveTag::BOOL);
			break;
		}

			// Addition
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::STAR:
		case TokenType::SLASH:
		case TokenType::MODULUS:
		{
			typeMatch = typeLhsP == typeRhsP && typeLhsP >= PT::INT;
			break;
		}

		default:
			typeMatch = false;
		}
	}

	if(typeMatch)
		expr->type = resType;
	else
		expr->type = TypeFactory::getNull();

	this->result = expr->type;
}

void TypeChecker::visit(ExprUnary* expr)
{
	auto typeRhs = expr->rhs->accept(this);
	// TODO implement with more details
	expr->type = typeRhs;
	this->result = expr->type;
}

void TypeChecker::visit(ExprLiteral* expr)
{
	expr->type = expr->literal.getType();
	this->result = expr->type;
}


