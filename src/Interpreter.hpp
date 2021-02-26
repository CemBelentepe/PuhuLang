#pragma once
#include "AstVisitor.hpp"
#include "Enviroment.hpp"
#include "Namespace.hpp"
#include "Value.hpp"
#include "Variable.hpp"

#include <unordered_map>

class NativeFunc;

class Interpreter : public ExprVisitor<Value>, public StmtVisitor<void>
{
public:
    class RunTimeVariable
    {
    public:
        Token name;
        std::shared_ptr<Type> type;
        bool initialized;
        Value val;

        RunTimeVariable() = default;
        RunTimeVariable(const RunTimeVariable& var);
        explicit RunTimeVariable(const Variable& var);
        explicit RunTimeVariable(const Token& name, const std::shared_ptr<Type>& type, bool initialized, const Value& val);
    };

public:
    Interpreter() = delete;
    explicit Interpreter(std::vector<std::unique_ptr<Stmt>>& root, const std::unique_ptr<Namespace<Variable>>& global);
    ~Interpreter() = default;

    void run();
    bool fail();
    void addNativeCallable(std::shared_ptr<NativeFunc> func);

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

    std::unique_ptr<Enviroment<RunTimeVariable>>& getCurrentEnviroment();

private:
    std::vector<std::unique_ptr<Stmt>>& root;
    std::unique_ptr<Namespace<RunTimeVariable>> global;
    Namespace<RunTimeVariable>* currentNamespace;
    std::unique_ptr<Enviroment<RunTimeVariable>> currentEnviroment;
    bool hadError;

private:
    static void init();

private:
    typedef Value (*UnaryFunction)(Value a);
    typedef Value (*BinaryFunction)(Value a, Value b);
    static std::unordered_map<TokenType, UnaryFunction> unaryFunctions;
    static std::unordered_map<TokenType, BinaryFunction> binaryFunctions;
};
