#pragma once
#include "AstVisitor.hpp"
#include <iostream>

class AstDebugger : public ExprVisitor<void>, public StmtVisitor<void>
{
public:
    AstDebugger() = delete;
    explicit AstDebugger(std::vector<std::unique_ptr<Stmt>>& root, std::ostream& os = std::cout);

    void debug();
    void showTypes(bool isShow);

    void visit(ExprLogic* expr) override;
    void visit(ExprBinary* expr) override;
    void visit(ExprUnary* expr) override;
    void visit(ExprCall* expr) override;
    void visit(ExprLiteral* expr) override;
    void visit(ExprHeap* expr) override;
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
    std::ostream& os;
    bool canShowType;
};
