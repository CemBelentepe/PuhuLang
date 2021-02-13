#pragma once
#include "AstVisitor.hpp"

#include <unordered_map>

class TypeChecker : public ExprVisitor<std::shared_ptr<Type>>
{
private:
    using PrimPtr = std::shared_ptr<TypePrimitive>;
public:
    TypeChecker() = delete;
    explicit TypeChecker(std::unique_ptr<Expr>& root);
    ~TypeChecker() = default;

    void check();
    bool fail();

    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprLiteral* expr) override;

private:
    std::unique_ptr<Expr>& root;
    bool hadError;

private:
    static void init();
    struct UnaryTransform
    {
        PrimPtr to;
        PrimPtr from;
        explicit UnaryTransform(PrimPtr to, PrimPtr from)
            : to(to), from(from)
        {
        }
    };
    struct BinaryTransform
    {
        PrimPtr to;
        PrimPtr lhs;
        PrimPtr rhs;
        explicit BinaryTransform(PrimPtr to, PrimPtr lhs, PrimPtr rhs)
            : to(to), lhs(lhs), rhs(rhs)
        {
        }
    };
    static std::unordered_map<TokenType, std::vector<UnaryTransform>> primitiveUnaryTransoforms;
    static std::unordered_map<TokenType, std::vector<BinaryTransform>> primitiveBinaryTransoforms;

    std::shared_ptr<Type> resolvePrimitiveUnary(Token& op, PrimPtr type_rhs);
    // std::shared_ptr<Type> resolveUserDefUnary(Token& op, TypeUserDef* type_rhs);
    std::shared_ptr<Type> resolvePrimitiveBinary(Token& op, PrimPtr type_lhs, PrimPtr type_rhs);
    // std::shared_ptr<Type> resolvePrimitiveUserDef(Token& op, TypePrimitive* type_lhs, TypePrimitive* type_rhs);

    class TypeError
    {
    public:
        std::string msg;
        Token token;
        explicit TypeError(const std::string& msg, Token token)
            : msg(msg), token(token) {}
        virtual ~TypeError() = default;

        virtual void log(std::ostream& os = std::cout)
        {
            os << "[ERROR] " << msg << " [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
    class UnaryTransformError : public TypeError
    {
    public:
        PrimPtr type;
        explicit UnaryTransformError(Token token, PrimPtr type)
            : TypeError("", token), type(type) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Operand of '" << token.lexeme << "' cannot be a type of '" << type->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
    class BinaryTransformError : public TypeError
    {
    public:
        PrimPtr type_lhs;
        PrimPtr type_rhs;
        explicit BinaryTransformError(Token token, PrimPtr type_lhs, PrimPtr type_rhs)
            : TypeError("", token), type_lhs(type_lhs), type_rhs(type_rhs) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Operands of '" << token.lexeme << "' cannot be the types of '" << type_lhs->toString() << "' and '" << type_rhs->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
};