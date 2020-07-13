#pragma once
#include "AstVisitor.hpp"

#include <string>
#include <sstream>

class TypeChecker : public AstVisitor
{
public:
    bool hadError;

    TypeChecker(Expr *expr)
        : hadError(false)
    {
        expr->accept(this);
    }

    void error(const std::string &message, Token token)
    {
        this->hadError = true;
        std::cout << "[Line " << token.line << ", " << token.getString() << "] " << message << std::endl;
    }

    void visit(ExprBinary *expr)
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
            if ((expr->left->type.tag == TypeTag::INTEGER || expr->left->type.tag == TypeTag::FLOAT || expr->left->type.tag == TypeTag::DOUBLE) 
                && (expr->right->type.tag == TypeTag::INTEGER || expr->right->type.tag == TypeTag::FLOAT || expr->right->type.tag == TypeTag::DOUBLE))
            {
                if (expr->left->type.tag > expr->right->type.tag)
                {
                    Type from = expr->right->type;
                    expr->right = new ExprCast(expr->left->type, expr->right);
                    ((ExprCast *)expr->right)->from = from;
                }
                else if (expr->right->type.tag > expr->left->type.tag)
                {
                    Type from = expr->left->type;
                    expr->left = new ExprCast(expr->right->type, expr->left);
                    ((ExprCast *)expr->left)->from = from;
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
                    ((ExprCast *)expr->left)->from = from;
                }
                if (expr->right->type.tag < TypeTag::DOUBLE)
                {
                    Type from = expr->right->type;
                    expr->right = new ExprCast(TypeTag::DOUBLE, expr->right);
                    ((ExprCast *)expr->right)->from = from;
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

    void visit(ExprCall *expr)
    {
        expr->callee->accept(this);

        if (expr->callee->type.tag != TypeTag::FUNCTION || expr->callee->type.tag != TypeTag::NATIVE)
            error("Only a type function can be called!", expr->openParen);

        for (auto &args : expr->args)
        {
            args->accept(this);
        }

        expr->type = expr->callee->type.intrinsicType;
    }
    void visit(ExprCast *expr)
    {
        expr->expr->accept(this);
        expr->from = expr->expr->type;
    }
    void visit(ExprLiteral *expr)
    {
        expr->type = expr->val->type;
    }
    void visit(ExprUnary *expr)
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
    void visit(ExprVariable *expr)
    {
        // TODO: find type from enviroment!!!
    }
};
