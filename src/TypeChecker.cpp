#include "TypeChecker.hpp"
#include <algorithm>
#include <memory>

std::unordered_map<TokenType, std::vector<TypeChecker::UnaryTransform>> TypeChecker::primitiveUnaryTransoforms;
std::unordered_map<TokenType, std::vector<TypeChecker::BinaryTransform>> TypeChecker::primitiveBinaryTransoforms;

TypeChecker::TypeChecker(std::vector<std::unique_ptr<Stmt>>& root)
    : root(root), hadError(false)
{
    init();
}

void TypeChecker::check()
{
    for (auto& stmt : root)
    {
        try
        {
            stmt->accept(this);
        }
        catch (TypeError& e)
        {
            e.log();
            hadError = true;
        }
    }
}

bool TypeChecker::fail()
{
    return hadError;
}

void TypeChecker::visit(ExprLogic* expr)
{
    auto type_lhs = expr->lhs->accept(this);
    auto type_rhs = expr->rhs->accept(this);

    if (type_lhs->tag != Type::Tag::PRIMITIVE || std::dynamic_pointer_cast<TypePrimitive>(type_lhs)->special_tag != TypePrimitive::PrimitiveTag::BOOL)
    {
        throw BinaryTransformError(expr->op, type_lhs, type_rhs);
    }
    if (type_rhs->tag != Type::Tag::PRIMITIVE || std::dynamic_pointer_cast<TypePrimitive>(type_rhs)->special_tag != TypePrimitive::PrimitiveTag::BOOL)
    {
        throw BinaryTransformError(expr->op, type_lhs, type_rhs);
    }

    result = expr->type;
}

void TypeChecker::visit(ExprBinary* expr)
{
    auto type_lhs = expr->lhs->accept(this);
    auto type_rhs = expr->rhs->accept(this);

    if (type_lhs->tag == Type::Tag::PRIMITIVE)
        expr->type = resolvePrimitiveBinary(expr->op, std::dynamic_pointer_cast<TypePrimitive>(type_lhs), std::dynamic_pointer_cast<TypePrimitive>(type_rhs));
    // else if (type_lhs->tag == Type::Tag::USER_DEF)
    //     expr->type = resolveUserDefBinary(expr->op, type_lhs, type_rhs);
    else
        throw BinaryTransformError(expr->op, type_lhs, type_rhs);

    result = expr->type;
}

void TypeChecker::visit(ExprUnary* expr)
{
    auto type_rhs = expr->rhs->accept(this);
    if (type_rhs->tag == Type::Tag::PRIMITIVE)
        expr->type = resolvePrimitiveUnary(expr->op, std::dynamic_pointer_cast<TypePrimitive>(type_rhs));
    // else if (type_rhs->tag == Type::Tag::USER_DEF)
    //     expr->type = resolveUserDefUnary(expr->op, type_rhs);
    else
        throw UnaryTransformError(expr->op, type_rhs);

    result = expr->type;
}

void TypeChecker::visit(ExprLiteral* expr)
{
    result = expr->type;
}

void TypeChecker::visit(StmtExpr* stmt)
{
    stmt->expr->accept(this);
}

void TypeChecker::init()
{
    static bool isInit = false;
    if (!isInit)
    {
        using tag = TypePrimitive::PrimitiveTag;
        isInit = true;
        auto makeUnary = [](tag a, tag b) {
            return UnaryTransform(std::make_shared<TypePrimitive>(a), std::make_shared<TypePrimitive>(b));
        };
        auto makeBinary = [](tag to, tag lhs, tag rhs) {
            return BinaryTransform(std::make_shared<TypePrimitive>(to), std::make_shared<TypePrimitive>(lhs), std::make_shared<TypePrimitive>(rhs));
        };

        // Unary
        primitiveUnaryTransoforms[TokenType::TILDE] = {makeUnary(tag::INT, tag::INT)};
        primitiveUnaryTransoforms[TokenType::BANG] = {makeUnary(tag::BOOL, tag::BOOL)};

        auto ifd = {makeUnary(tag::INT, tag::INT),
                    makeUnary(tag::FLOAT, tag::FLOAT),
                    makeUnary(tag::DOUBLE, tag::DOUBLE)};
        primitiveUnaryTransoforms[TokenType::MINUS] = ifd;
        primitiveUnaryTransoforms[TokenType::PLUS] = ifd;
        primitiveUnaryTransoforms[TokenType::MINUS_MINUS] = ifd;
        primitiveUnaryTransoforms[TokenType::PLUS_PLUS] = ifd;

        // Binary
        auto iii = {makeBinary(tag::INT, tag::INT, tag::INT)};
        primitiveBinaryTransoforms[TokenType::BIT_OR] = iii;
        primitiveBinaryTransoforms[TokenType::BIT_XOR] = iii;
        primitiveBinaryTransoforms[TokenType::BIT_AND] = iii;
        primitiveBinaryTransoforms[TokenType::BITSHIFT_LEFT] = iii;
        primitiveBinaryTransoforms[TokenType::BITSHIFT_RIGHT] = iii;
        primitiveBinaryTransoforms[TokenType::MODULUS] = iii;

        auto comparison = {
            makeBinary(tag::BOOL, tag::BOOL, tag::BOOL),
            makeBinary(tag::BOOL, tag::CHAR, tag::CHAR),
            makeBinary(tag::BOOL, tag::INT, tag::INT),
            makeBinary(tag::BOOL, tag::FLOAT, tag::FLOAT),
            makeBinary(tag::BOOL, tag::DOUBLE, tag::DOUBLE)};
        primitiveBinaryTransoforms[TokenType::EQUAL_EQUAL] = comparison;
        primitiveBinaryTransoforms[TokenType::BANG_EQUAL] = comparison;
        primitiveBinaryTransoforms[TokenType::LESS] = comparison;
        primitiveBinaryTransoforms[TokenType::LESS_EQUAL] = comparison;
        primitiveBinaryTransoforms[TokenType::GREAT] = comparison;
        primitiveBinaryTransoforms[TokenType::GREAT_EQUAL] = comparison;

        auto arith = {
            makeBinary(tag::INT, tag::INT, tag::INT),
            makeBinary(tag::FLOAT, tag::FLOAT, tag::FLOAT),
            makeBinary(tag::DOUBLE, tag::DOUBLE, tag::DOUBLE)};
        primitiveBinaryTransoforms[TokenType::PLUS] = arith;
        primitiveBinaryTransoforms[TokenType::MINUS] = arith;
        primitiveBinaryTransoforms[TokenType::STAR] = arith;
        primitiveBinaryTransoforms[TokenType::SLASH] = arith;
    }
}

std::shared_ptr<Type> TypeChecker::resolvePrimitiveUnary(Token& op, PrimPtr type_rhs)
{
    auto it_type = primitiveUnaryTransoforms.find(op.type);
    if (it_type == primitiveUnaryTransoforms.end())
        throw TypeError("Operator is not implemented yet.", op);

    auto it = std::find_if(it_type->second.begin(), it_type->second.end(), [&](UnaryTransform t) { return t.from->special_tag == type_rhs->special_tag; });
    if (it == it_type->second.end())
        throw UnaryTransformError(op, type_rhs);

    return it->to;
}

std::shared_ptr<Type> TypeChecker::resolvePrimitiveBinary(Token& op, PrimPtr type_lhs, PrimPtr type_rhs)
{
    auto it_type = primitiveBinaryTransoforms.find(op.type);
    if (it_type == primitiveBinaryTransoforms.end())
        throw TypeError("Operator is not implemented yet.", op);

    auto it = std::find_if(it_type->second.begin(), it_type->second.end(), [&](BinaryTransform t) { return t.lhs->special_tag == type_lhs->special_tag && t.rhs->special_tag == type_rhs->special_tag; });
    if (it == it_type->second.end())
        throw BinaryTransformError(op, type_lhs, type_rhs);

    return it->to;
}
