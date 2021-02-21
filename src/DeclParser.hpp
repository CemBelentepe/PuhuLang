#pragma once
#include "Stmt.hpp"
#include "AstVisitor.hpp"
#include "Namespace.hpp"
#include "Variable.hpp"

#include <vector>

class DeclParser : public StmtVisitor<void>
{
public:
    DeclParser() = delete;
    explicit DeclParser(std::vector<std::unique_ptr<Stmt>>& root);
    ~DeclParser() = default;

    std::unique_ptr<Namespace<Variable>> parse();
    bool fail();

    void visit(StmtExpr* stmt) override;
    void visit(StmtBody* stmt) override;
    void visit(StmtReturn* stmt) override;
    void visit(StmtIf* stmt) override;
    void visit(StmtWhile* stmt) override;
    void visit(DeclVar* decl) override;
    void visit(DeclFunc* decl) override;

private:
    std::unique_ptr<Namespace<Variable>> global;
    Namespace<Variable>* currentNamespace;
    std::vector<std::unique_ptr<Stmt>>& root;
    bool hadError;
};
