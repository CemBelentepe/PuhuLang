#pragma once

#include "AstVisitor.hpp"
#include "Namespace.hpp"
#include "Enviroment.hpp"
#include "Variable.hpp"

#include <unordered_map>

class TypeChecker : public ExprVisitor<std::shared_ptr<Type>>, public StmtVisitor<void>
{
private:
    using PrimPtr = std::shared_ptr<TypePrimitive>;
    using TypePtr = std::shared_ptr<Type>;
    
public:
    TypeChecker() = delete;
    explicit TypeChecker(std::vector<std::unique_ptr<Stmt>>& root, std::unique_ptr<Namespace<Variable>>& global);
    ~TypeChecker() = default;

    void check();
    bool fail();

    void visit(ExprLogic* expr) override;
    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprCall* expr) override;
    void visit(ExprLiteral* expr) override;
    void visit(ExprVariableGet* expr) override;
    void visit(ExprVariableSet* expr) override;

    void visit(StmtExpr* stmt) override;
    void visit(StmtBody* stmt) override;
    void visit(StmtReturn* stmt) override;
    void visit(StmtIf* stmt) override;
    void visit(StmtWhile* stmt) override;
    void visit(DeclVar* decl) override;
    void visit(DeclFunc* decl) override;
    void visit(DeclNamespace* decl) override;

private:
    std::vector<std::unique_ptr<Stmt>>& root;
    std::unique_ptr<Namespace<Variable>>& global;
    Namespace<Variable>* currentNamespace;
    std::unique_ptr<Enviroment<Variable>> currentEnviroment;
    std::shared_ptr<Type> currentReturn;
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
        TypePtr type;
        explicit UnaryTransformError(Token token, TypePtr type)
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
        TypePtr type_lhs;
        TypePtr type_rhs;
        explicit BinaryTransformError(Token token, TypePtr type_lhs, TypePtr type_rhs)
            : TypeError("", token), type_lhs(type_lhs), type_rhs(type_rhs) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Operands of '" << token.lexeme << "' cannot be the types of '" << type_lhs->toString() << "' and '" << type_rhs->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
    class AssignmentError : public TypeError
    {
    public:
        TypePtr type_lhs;
        TypePtr type_rhs;
        explicit AssignmentError(Token token, TypePtr type_lhs, TypePtr type_rhs)
            : TypeError("", token), type_lhs(type_lhs), type_rhs(type_rhs) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Cannot assign a type of '" << type_rhs->toString() << "' to a type of '" << type_lhs->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
    class ReturnError : public TypeError
    {
    public:
        TypePtr type_expected;
        TypePtr type_returning;
        explicit ReturnError(Token token, TypePtr type_expected, TypePtr type_returning)
            : TypeError("", token), type_expected(type_expected), type_returning(type_returning) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Cannot return a type of '" << type_returning->toString() << "' from a function returning '" << type_expected->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
    class NotExpectedError : public TypeError
    {
    public:
        TypePtr type_expected;
        TypePtr type_recieved;
        explicit NotExpectedError(Token token, TypePtr type_expected, TypePtr type_recieved)
            : TypeError("", token), type_expected(type_expected), type_recieved(type_recieved) {}

        virtual void log(std::ostream& os = std::cout) override
        {
            os << "[ERROR] Expected the type of '" << type_expected->toString() << "' but recieved '" << type_recieved->toString() << "' [line: " << token.line << ", col: " << token.col << "]\n";
            os << token.toStringInLine(Logger::RED);
        }
    };
};
