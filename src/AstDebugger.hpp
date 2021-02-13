#pragma once
#include "AstVisitor.hpp"
#include <iostream>

class AstDebugger : public ExprVisitor<void>
{
private:
    std::unique_ptr<Expr>& root;
    std::ostream& os;
    bool isShow;
public:
    AstDebugger() = delete;
    explicit AstDebugger(std::unique_ptr<Expr>& root, std::ostream& os = std::cout);

    void debug();
    void showTypes(bool isShow);
    
    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprLiteral* expr) override;
};
