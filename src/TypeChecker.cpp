#include "TypeChecker.hpp"

#include <algorithm>
#include <memory>
#include <sstream>

std::unordered_map<TokenType, std::vector<TypeChecker::UnaryTransform>> TypeChecker::primitiveUnaryTransoforms;
std::unordered_map<TokenType, std::vector<TypeChecker::BinaryTransform>> TypeChecker::primitiveBinaryTransoforms;

TypeChecker::TypeChecker(std::vector<std::unique_ptr<Stmt>>& root, std::unique_ptr<Namespace<Variable>>& global)
    : root(root), hadError(false), global(global), currentNamespace(global.get()), currentEnviroment(nullptr), currentReturn(nullptr)
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
        catch (const Parser::TokenError& e)
        {
            std::cout << e << std::endl;
            hadError = true;
        }
    }

    currentNamespace = global.get();
    Variable entry;
    try
    {
        entry = currentNamespace->getVariable("main");
    }
    catch (const std::string& err)
    {
        std::cout << err << std::endl;
        hadError = true;
        return;
    }

    if (!entry.type->isSame(std::make_shared<TypeFunction>(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::VOID))))
    {
        std::cout << "Expected type 'void()' for the entry function main but recieved '" << entry.type->toString() << "'." << std::endl;
        hadError = true;
        return;
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

void TypeChecker::visit(ExprCall* expr)
{
    std::vector<std::shared_ptr<Type>> arg_types;
    std::for_each(expr->args.begin(), expr->args.end(), [&](auto& arg) {
        arg_types.emplace_back(arg->accept(this));
    });

    auto type_callee = expr->callee->accept(this);
    if (type_callee->tag == Type::Tag::FUNCTION)
    {
        auto param_types = std::dynamic_pointer_cast<TypeFunction>(type_callee)->param_types;
        if (param_types.size() != arg_types.size())
        {
            std::stringstream ss;
            ss << "Expected " << param_types.size() << " number of arguments but recieved " << arg_types.size() << " arguments.";
            throw TypeError(ss.str(), expr->paren);
        }
        for (size_t i = 0; i < param_types.size(); i++)
        {
            if (!param_types[i]->isSame(arg_types[i]))
            {
                std::stringstream ss;
                ss << "Expected type of '" << param_types[i]->toString() << "' for the element " << i << " but recieved '" << arg_types[i]->toString() << "'.";
                throw TypeError(ss.str(), expr->paren);
            }
        }
        expr->type = type_callee->instrinsicType;
        result = type_callee->instrinsicType;
    }
    else
    {
        throw TypeError("Expression of the callee does not evaluate to a callable.", expr->paren);
    }
}

void TypeChecker::visit(ExprLiteral* expr)
{
    result = expr->type;
}

void TypeChecker::visit(ExprVariableGet* expr)
{
    if (currentEnviroment)
    {
        try
        {
            expr->type = currentEnviroment->getVariable(expr->name).type;
        }
        catch (const Parser::TokenError& err)
        {
            expr->type = currentNamespace->getVariable(expr->name).type;
        }
    }
    else
    {
        expr->type = currentNamespace->getVariable(expr->name).type;
    }
    result = expr->type;
}

void TypeChecker::visit(ExprVariableSet* expr)
{
    std::shared_ptr<Type> type_asgn = expr->asgn->accept(this);

    Variable var;
    if (currentEnviroment)
    {
        try
        {
            var = currentEnviroment->getVariable(expr->name);
        }
        catch (const Parser::TokenError& err)
        {
            var = currentNamespace->getVariable(expr->name);
        }
    }
    else
    {
        var = currentNamespace->getVariable(expr->name);
    }

    if (!var.type->isSame(type_asgn))
        throw AssignmentError(expr->equal, var.type, type_asgn);

    expr->type = var.type;
    result = expr->type;
}

void TypeChecker::visit(StmtExpr* stmt)
{
    stmt->expr->accept(this);
}

void TypeChecker::visit(StmtBody* stmt)
{
    currentEnviroment = std::make_unique<Enviroment<Variable>>(std::move(currentEnviroment));

    for (auto& s : stmt->body)
    {
        s->accept(this);
    }

    currentEnviroment = currentEnviroment->returnToParent();
}

void TypeChecker::visit(StmtReturn* stmt)
{
    std::shared_ptr<Type> retType;
    if (stmt->retExpr)
        retType = stmt->retExpr->accept(this);
    else
        retType = std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::VOID);
        
    if (!retType->isSame(currentReturn))
    {
        throw ReturnError(stmt->retToken, currentReturn, retType);
    }
}

void TypeChecker::visit(DeclVar* decl)
{
    if (currentEnviroment)
    {
        currentEnviroment->addVariable(Variable(decl->name, decl->type, decl->initter != nullptr)); // define local
    }

    if (decl->type->isSame(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::VOID)))
    {
        throw Parser::TokenError("A variable cannot be a type of 'void'.", decl->equal);
    }

    if (decl->initter)
    {
        decl->initter->accept(this);

        if (!decl->initter->type->isSame(decl->type))
        {
            throw AssignmentError(decl->equal, decl->type, decl->initter->type);
        }
    }
}

void TypeChecker::visit(DeclFunc* decl)
{
    currentEnviroment = std::make_unique<Enviroment<Variable>>(std::move(currentEnviroment));

    auto savedReturn = currentReturn;
    currentReturn = decl->type->instrinsicType;

    const std::vector<std::shared_ptr<Type>>& param_types = std::dynamic_pointer_cast<TypeFunction>(decl->type)->param_types;
    for (size_t i = 0; i < decl->param_names.size(); i++)
    {
        currentEnviroment->addVariable(Variable(decl->param_names[i], param_types[i], true));
    }

    decl->body->accept(this);

    currentReturn = savedReturn;
    currentEnviroment = currentEnviroment->returnToParent();
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
