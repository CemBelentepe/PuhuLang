#pragma once
#include "AstVisitor.hpp"
#include "Value.hpp"

#include <unordered_map>

class Interpreter : public ExprVisitor<Value>
{
private:
    std::unique_ptr<Expr>& root;
    bool hadError;
public:
    Interpreter() = delete;
    explicit Interpreter(std::unique_ptr<Expr>& root);
    ~Interpreter() = default;
    
    void run();
    bool fail();

    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprLiteral* expr) override;

private: 
    static void init();

private:
    typedef Value(*UnaryFunction)(Value a);
    typedef Value(*BinaryFunction)(Value a, Value b);
    static std::unordered_map<TokenType, UnaryFunction> unaryFunctions;
    static std::unordered_map<TokenType, BinaryFunction> binaryFunctions;
};