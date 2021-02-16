#pragma once
#include "AstVisitor.hpp"
#include "Value.hpp"

#include <unordered_map>

class Interpreter : public ExprVisitor<Value>, public StmtVisitor<void>
{
public:
    Interpreter() = delete;
    explicit Interpreter(std::vector<std::unique_ptr<Stmt>>& root);
    ~Interpreter() = default;

    void run();
    bool fail();

    void visit(ExprLogic* expr) override;
    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprLiteral* expr) override;

    void visit(StmtExpr* stmt) override;

private:
    std::vector<std::unique_ptr<Stmt>>& root;
    bool hadError;

private:
    static void init();

private:
    typedef Value (*UnaryFunction)(Value a);
    typedef Value (*BinaryFunction)(Value a, Value b);
    static std::unordered_map<TokenType, UnaryFunction> unaryFunctions;
    static std::unordered_map<TokenType, BinaryFunction> binaryFunctions;
};