#pragma once

#include "AstVisitor.hpp"
#include "Enviroment.hpp"

#include <sstream>
#include <string>

class TypeChecker : public AstVisitor
{
public:
    bool hadError;
    std::unordered_map<std::string, Value*>& globals;
    Enviroment* currentEnviroment;

    TypeChecker(std::vector<Stmt*>& root, std::unordered_map<std::string, Value*>& globals)
        : hadError(false), globals(globals), currentEnviroment(new Enviroment(nullptr, 0))
    {
        for (auto& val : globals)
            currentEnviroment->define(val.first, val.second->type);

        for (auto& stmt : root)
            stmt->accept(this);
    }

    void error(const std::string& message, Token token)
    {
        this->hadError = true;
        std::cout << "[Line " << token.line << ", " << token.getString() << "] " << message << std::endl;
    }

    void beginScope()
    {
        currentEnviroment = new Enviroment(currentEnviroment, 0);
    }

    void endScope()
    {
        std::vector<TypeTag> types = currentEnviroment->getEnvTypes();
        Enviroment* env = currentEnviroment->closing;
        delete currentEnviroment;
        currentEnviroment = env;
    }

    void visit(ExprAssignment* expr)
    {
        expr->assignment->accept(this);
        Type type = currentEnviroment->get(expr->name).type;
        expr->type = type;
        if (expr->assignment->type != type)
        {
            std::stringstream ss;
            ss << "Invalid assignment due to type missmatch, variable '" << expr->name.getString() << "' has the type '" << type << "' but the expression type is '" << expr->assignment->type;
            error(ss.str(), expr->name);
        }
    }

    void visit(ExprBinary* expr)
    {
        expr->left->accept(this);
        expr->right->accept(this);

        switch (expr->op.type)
        {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
        {
            if ((expr->left->type.tag == TypeTag::INTEGER || expr->left->type.tag == TypeTag::FLOAT || expr->left->type.tag == TypeTag::DOUBLE) && (expr->right->type.tag == TypeTag::INTEGER || expr->right->type.tag == TypeTag::FLOAT || expr->right->type.tag == TypeTag::DOUBLE))
            {
                if (expr->left->type.tag > expr->right->type.tag)
                {
                    Type from = expr->right->type;
                    expr->right = new ExprCast(expr->left->type, expr->right);
                    ((ExprCast*)expr->right)->from = from;
                }
                else if (expr->right->type.tag > expr->left->type.tag)
                {
                    Type from = expr->left->type;
                    expr->left = new ExprCast(expr->right->type, expr->left);
                    ((ExprCast*)expr->left)->from = from;
                }
                expr->type = expr->left->type;
            }
            else
            {
                std::stringstream ss;
                ss << "Both operands of '" << expr->op.getString() << "' must be a type of 'int', 'float' or 'double'.";
                error(ss.str(), expr->op);
            }

            break;
        }
        case TokenType::MODULUS:
        case TokenType::BIT_OR:
        case TokenType::BIT_XOR:
        case TokenType::BIT_AND:
        case TokenType::BITSHIFT_LEFT:
        case TokenType::BITSHIFT_RIGHT:
        {
            if (expr->left->type != TypeTag::INTEGER || expr->right->type != TypeTag::INTEGER)
            {
                std::stringstream ss;
                ss << "Both operands of '" << expr->op.getString() << "' must be a type of 'int'.";
                error(ss.str(), expr->op);
            }
            else
                expr->type = TypeTag::INTEGER;
            break;
        }
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREAT:
        case TokenType::GREAT_EQUAL:
        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
        {
            if (expr->left->type == TypeTag::INTEGER && expr->right->type == TypeTag::INTEGER)
                expr->type = TypeTag::BOOL;
            else if (expr->left->type.tag <= TypeTag::DOUBLE && expr->right->type.tag <= TypeTag::DOUBLE)
            {
                if (expr->left->type.tag < TypeTag::DOUBLE)
                {
                    Type from = expr->left->type;
                    expr->left = new ExprCast(TypeTag::DOUBLE, expr->left);
                    ((ExprCast*)expr->left)->from = from;
                }
                if (expr->right->type.tag < TypeTag::DOUBLE)
                {
                    Type from = expr->right->type;
                    expr->right = new ExprCast(TypeTag::DOUBLE, expr->right);
                    ((ExprCast*)expr->right)->from = from;
                }

                expr->type = TypeTag::BOOL;
            }
            else
            {
                std::stringstream ss;
                ss << "Both operands of '" << expr->op.getString() << "' must be a type of 'int', 'float' or 'double'.";
                error(ss.str(), expr->op);
            }

            break;
        }
        }
    }

    void visit(ExprCall* expr)
    {
        expr->callee->accept(this);

        if (expr->callee->type.tag != TypeTag::FUNCTION && expr->callee->type.tag != TypeTag::NATIVE)
            error("Only a type function can be called!", expr->openParen);

        // TODO: Compare with table
        for (auto& args : expr->args)
        {
            args->accept(this);
        }

        expr->type = expr->callee->type.intrinsicType;
    }

    void visit(ExprCast* expr)
    {
        expr->expr->accept(this);
        expr->from = expr->expr->type;
    }

    void visit(ExprLiteral* expr)
    {
        expr->type = expr->val->type;
    }

    void visit(ExprLogic* expr)
    {
        expr->left->accept(this);
        expr->right->accept(this);
        if (expr->left->type != TypeTag::BOOL || expr->right->type != TypeTag::BOOL)
        {
            std::stringstream ss;
            ss << "Both operands of '" << expr->op.getString() << "' must be a type of 'bool'.";
            error(ss.str(), expr->op);
        }
    }

    void visit(ExprUnary* expr)
    {
        expr->expr->accept(this);
        expr->type = expr->expr->type;

        std::stringstream ss;
        ss << "Operand of '" << expr->op.getString() << "' must be a type of '";

        switch (expr->op.type)
        {
        case TokenType::MINUS:
        case TokenType::PLUS_PLUS:
        case TokenType::MINUS_MINUS:
            if (expr->type.tag != TypeTag::INTEGER && expr->type.tag != TypeTag::FLOAT && expr->type.tag != TypeTag::DOUBLE)
            {
                ss << "int', 'float' or 'double'.";
                error(ss.str(), expr->op);
            }
            break;

        case TokenType::BANG:
            if (expr->type.tag != TypeTag::BOOL)
            {
                ss << "bool'.";
                error(ss.str(), expr->op);
            }
            break;

        case TokenType::TILDE:
            if (expr->type.tag != TypeTag::INTEGER)
            {
                ss << "int'.";
                error(ss.str(), expr->op);
            }
            break;
        }
    }

    void visit(ExprVariable* expr)
    {
        Variable var = currentEnviroment->get(expr->name);
        if (var.depth == 0)
            expr->type = globals[expr->name.getString()]->type;
        else
            expr->type = var.type;
    }

    void visit(StmtBlock* stmt)
    {
        beginScope();
        for (auto& s : stmt->statements)
            s->accept(this);
        endScope();

        this->hadError = false;
    }
    void visit(StmtExpr* stmt)
    {
        stmt->expr->accept(this);
        this->hadError = false;
    }
    void visit(StmtFunc* stmt)
    {
        beginScope();
        for (auto& s : stmt->args)
            currentEnviroment->define(s.first, s.second);

        for (auto& s : stmt->body->statements)
            s->accept(this);

        endScope();
    }
    void visit(StmtVarDecleration* stmt)
    {
        if (stmt->initializer != nullptr)
        {
            stmt->initializer->accept(this);
            if (stmt->varType != stmt->initializer->type)
                error("Type of the initializer of the variable is not same as the variable type.", stmt->name);
        }

        if (currentEnviroment->depth != 0)
            currentEnviroment->define(stmt->name, stmt->varType);
    }
    void visit(StmtReturn* stmt)
    {
        // TODO: add type checking of the return value
        if (stmt->retVal != nullptr)
        {
            stmt->retVal->accept(this);
        }
    }
    void visit(StmtIf* stmt)
    {
        stmt->condition->accept(this);
        if (stmt->condition->type.tag != TypeTag::BOOL)
            error("Type of the if condition must be a 'bool'", stmt->paren);
        stmt->then->accept(this);
        if (stmt->els != nullptr)
            stmt->els->accept(this);
    }
    void visit(StmtFor* stmt)
    {
        beginScope();
        if (stmt->decl)
            stmt->decl->accept(this);
        if (stmt->cond)
            stmt->cond->accept(this);
        if (stmt->inc)
            stmt->inc->accept(this);
        stmt->loop->accept(this);
        endScope();
    }
    void visit(StmtWhile* stmt)
    {
        stmt->condition->accept(this);
        if (stmt->condition->type.tag != TypeTag::BOOL)
            error("Type of the while condition must be a 'bool'", stmt->paren);
        stmt->loop->accept(this);
    }
};
