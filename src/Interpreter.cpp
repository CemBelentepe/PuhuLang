#include "Interpreter.hpp"
#include "Callable.hpp"
#include "Native/Native.hpp"

#include <algorithm>

std::unordered_map<TokenType, Interpreter::UnaryFunction> Interpreter::unaryFunctions;
std::unordered_map<TokenType, Interpreter::BinaryFunction> Interpreter::binaryFunctions;

Interpreter::RunTimeVariable::RunTimeVariable(const RunTimeVariable& var)
    : name(var.name), type(var.type), initialized(var.initialized), val(var.val)
{
}

Interpreter::RunTimeVariable::RunTimeVariable(const Variable& var)
    : name(var.name), type(var.type), initialized(false), val(Value())
{
}

Interpreter::RunTimeVariable::RunTimeVariable(const Token& name, const std::shared_ptr<Type>& type, bool initialized, const Value& val)
    : name(name), type(type), initialized(initialized), val(val)
{
}

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmt>>& root, const std::unique_ptr<Namespace<Variable>>& global)
    : root(root), hadError(false), global(std::make_unique<Namespace<RunTimeVariable>>(global, nullptr)), currentEnviroment(nullptr)
{
    init();
    NativeFunc::InitTo(this);
    currentNamespace = this->global.get();
}

void Interpreter::run()
{
    for (auto& stmt : root)
        stmt->accept(this);

    currentNamespace = global.get();
    RunTimeVariable entry;
    try
    {
        entry = currentNamespace->getVariable("main");
    }
    catch (const std::string& err)
    {
        std::cout << err << '\n';
        hadError = true;
        return;
    }

    try
    {
        std::get<std::shared_ptr<Callable>>(entry.val.data.data)->call(this, {});
    }
    catch (const Parser::TokenError& err)
    {
        std::cout << err << std::endl;
        hadError = true;
    }
}

bool Interpreter::fail()
{
    return hadError;
}

void Interpreter::addNativeCallable(std::shared_ptr<NativeFunc> func)
{
    global->getVariable(func->getNamespace(), func->tokenName()).val = Value(func);
}

void Interpreter::visit(ExprLogic* expr)
{
    Value lhs = expr->lhs->accept(this);
    if (expr->op.type == TokenType::OR)
    {
        if (std::get<bool>(lhs.data.data))
            result = Value(true);
        else
            result = expr->rhs->accept(this);
    }
    else if (expr->op.type == TokenType::AND)
    {
        if (std::get<bool>(lhs.data.data))
            result = expr->rhs->accept(this);
        else
            result = Value(false);
    }
}

void Interpreter::visit(ExprBinary* expr)
{
    Value lhs = expr->lhs->accept(this);
    Value rhs = expr->rhs->accept(this);
    if (expr->type->tag == Type::Tag::PRIMITIVE)
        result = binaryFunctions[expr->op.type](lhs, rhs);
    else if (expr->type->tag == Type::Tag::STRING)
        result = Value(std::get<std::string>(lhs.data.data) + std::get<std::string>(rhs.data.data));
}

void Interpreter::visit(ExprUnary* expr)
{
    Value rhs = expr->rhs->accept(this);
    result = unaryFunctions[expr->op.type](rhs);
}

void Interpreter::visit(ExprCall* expr)
{
    std::vector<Value> args;
    std::for_each(expr->args.begin(), expr->args.end(), [&](auto& arg) {
        args.emplace_back(arg->accept(this));
    });

    Value callee = expr->callee->accept(this);
    auto callable = std::get<std::shared_ptr<Callable>>(callee.data.data);

    auto prevEnv = std::move(currentEnviroment);
    currentEnviroment = std::make_unique<Enviroment<Interpreter::RunTimeVariable>>(nullptr);
    auto prevNs = currentNamespace;
    currentNamespace = global->getNamespace(callable->getNamespace());
    result = callable->call(this, args);
    currentNamespace = prevNs;
    currentEnviroment = std::move(prevEnv);
}

void Interpreter::visit(ExprLiteral* expr)
{
    result = expr->value;
}

void Interpreter::visit(ExprHeap* expr)
{
    result = Value(new Data());
}

void Interpreter::visit(ExprVariableGet* expr)
{
    if (expr->address.size() > 0)
    {
        result = currentNamespace->getVariable(expr->address, expr->name).val;
    }
    else
    {
        if (currentEnviroment)
        {
            try
            {
                result = currentEnviroment->getVariable(expr->name).val;
            }
            catch (const Parser::TokenError& err)
            {
                result = currentNamespace->getVariable(expr->name).val;
            }
        }
        else
        {
            result = currentNamespace->getVariable(expr->name).val;
        }
    }
}

void Interpreter::visit(ExprVariableSet* expr)
{
    Value val = expr->asgn->accept(this);
    if (expr->address.size() > 0)
    {
        currentNamespace->getVariable(expr->address, expr->name).val = val;
    }
    else
    {
        if (currentEnviroment)
        {
            try
            {
                currentEnviroment->getVariable(expr->name).val = val;
            }
            catch (const Parser::TokenError& err)
            {
                currentNamespace->getVariable(expr->name).val = val;
            }
        }
        else
        {
            currentNamespace->getVariable(expr->name).val = val;
        }
    }
    result = val;
}

void Interpreter::visit(StmtExpr* stmt)
{
    stmt->expr->accept(this);
    // Value res = stmt->expr->accept(this);
    // std::cout << res << std::endl;
}

void Interpreter::visit(StmtBody* stmt)
{
    currentEnviroment = std::make_unique<Enviroment<RunTimeVariable>>(std::move(currentEnviroment));

    for (auto& s : stmt->body)
    {
        s->accept(this);
    }

    currentEnviroment = currentEnviroment->returnToParent();
}

void Interpreter::visit(StmtReturn* stmt)
{
    Value retVal;
    if (stmt->retExpr)
    {
        retVal = stmt->retExpr->accept(this);
    }
    throw retVal;
}

void Interpreter::visit(StmtIf* stmt)
{
    Value cond = stmt->cond->accept(this);
    if (std::get<bool>(cond.data.data))
    {
        stmt->then->accept(this);
    }
    else if (stmt->els)
    {
        stmt->els->accept(this);
    }
}

void Interpreter::visit(StmtWhile* stmt)
{
    Value cond = stmt->cond->accept(this);
    while (std::get<bool>(cond.data.data))
    {
        stmt->body->accept(this);
        cond = stmt->cond->accept(this);
    }
}

void Interpreter::visit(DeclVar* decl)
{
    if (decl->initter)
    {
        Value val = decl->initter->accept(this);

        if (currentEnviroment)
        {
            currentEnviroment->addVariable(Interpreter::RunTimeVariable(decl->name, val.type, true, val));
        }
        else
        {
            RunTimeVariable& var = currentNamespace->getVariable(decl->name);
            var.val = val;
            var.initialized = true;
        }
    }
    else if (currentEnviroment)
    {
        currentEnviroment->addVariable(Interpreter::RunTimeVariable(decl->name, decl->type, false, Value()));
    }
}

void Interpreter::visit(DeclFunc* decl)
{
    // TODO do before start
    currentNamespace->getVariable(decl->name).val = Value(std::make_shared<PuhuFunction>(decl));
}

void Interpreter::visit(DeclNamespace* decl)
{
    // TODO do before start
    currentNamespace = currentNamespace->getChild(decl->name);
    for (auto& stmt : decl->body)
    {
        stmt->accept(this);
    }
    currentNamespace = currentNamespace->parent;
}

std::unique_ptr<Enviroment<Interpreter::RunTimeVariable>>& Interpreter::getCurrentEnviroment()
{
    return currentEnviroment;
}

void Interpreter::init()
{
#define UNARY_(op, val, type) Value(op std::get<type>((val).data.data))
#define UNARY_IFD(tokenType, op)                                          \
    unaryFunctions[TokenType::tokenType] = [](Value a) {                  \
        TypePrimitive* type = dynamic_cast<TypePrimitive*>(a.type.get()); \
        switch (type->special_tag)                                        \
        {                                                                 \
        case tag::INT:                                                    \
            return UNARY_(op, a, int);                                    \
        case tag::FLOAT:                                                  \
            return UNARY_(op, a, int);                                    \
        case tag::DOUBLE:                                                 \
            return UNARY_(op, a, int);                                    \
        default:                                                          \
            return Value();                                               \
        }                                                                 \
    };

#define BINARY_(op, lhs, rhs, type) Value(std::get<type>((lhs).data.data) op std::get<type>((rhs).data.data))
#define BINARY_COMP(tokenType, op)                                          \
    binaryFunctions[TokenType::tokenType] = [](Value lhs, Value rhs) {      \
        TypePrimitive* type = dynamic_cast<TypePrimitive*>(lhs.type.get()); \
        switch (type->special_tag)                                          \
        {                                                                   \
        case tag::BOOL:                                                     \
            return BINARY_(op, lhs, rhs, bool);                             \
        case tag::CHAR:                                                     \
            return BINARY_(op, lhs, rhs, char);                             \
        case tag::INT:                                                      \
            return BINARY_(op, lhs, rhs, int);                              \
        case tag::FLOAT:                                                    \
            return BINARY_(op, lhs, rhs, float);                            \
        case tag::DOUBLE:                                                   \
            return BINARY_(op, lhs, rhs, double);                           \
        default:                                                            \
            return Value();                                                 \
        }                                                                   \
    };
#define BINARY_ARITH(tokenType, op)                                         \
    binaryFunctions[TokenType::tokenType] = [](Value lhs, Value rhs) {      \
        TypePrimitive* type = dynamic_cast<TypePrimitive*>(lhs.type.get()); \
        switch (type->special_tag)                                          \
        {                                                                   \
        case tag::INT:                                                      \
            return BINARY_(op, lhs, rhs, int);                              \
        case tag::FLOAT:                                                    \
            return BINARY_(op, lhs, rhs, float);                            \
        case tag::DOUBLE:                                                   \
            return BINARY_(op, lhs, rhs, double);                           \
        default:                                                            \
            return Value();                                                 \
        }                                                                   \
    };

    static bool isInit = false;
    if (!isInit)
    {
        isInit = true;
        using tag = TypePrimitive::PrimitiveTag;

        // Unary
        unaryFunctions[TokenType::BANG] = [](Value a) {
            return UNARY_(!, a, bool);
        };
        unaryFunctions[TokenType::TILDE] = [](Value a) {
            return UNARY_(!, a, bool);
        };

        UNARY_IFD(MINUS, -)
        UNARY_IFD(PLUS, +)
        UNARY_IFD(MINUS_MINUS, --)
        UNARY_IFD(PLUS_PLUS, ++)

        // Binary
        binaryFunctions[TokenType::BIT_OR] = [](Value lhs, Value rhs) {
            return BINARY_(|, lhs, rhs, int);
        };
        binaryFunctions[TokenType::BIT_XOR] = [](Value lhs, Value rhs) {
            return BINARY_(^, lhs, rhs, int);
        };
        binaryFunctions[TokenType::BIT_AND] = [](Value lhs, Value rhs) {
            return BINARY_(&, lhs, rhs, int);
        };
        binaryFunctions[TokenType::BITSHIFT_LEFT] = [](Value lhs, Value rhs) {
            return BINARY_(<<, lhs, rhs, int);
        };
        binaryFunctions[TokenType::BITSHIFT_RIGHT] = [](Value lhs, Value rhs) {
            return BINARY_(>>, lhs, rhs, int);
        };
        binaryFunctions[TokenType::MODULUS] = [](Value lhs, Value rhs) {
            return BINARY_(%, lhs, rhs, int);
        };

        BINARY_COMP(EQUAL_EQUAL, ==)
        BINARY_COMP(BANG_EQUAL, !=)
        BINARY_COMP(LESS, <)
        BINARY_COMP(LESS_EQUAL, <=)
        BINARY_COMP(GREAT, >)
        BINARY_COMP(GREAT_EQUAL, >=)

        BINARY_ARITH(PLUS, +)
        BINARY_ARITH(MINUS, -)
        BINARY_ARITH(STAR, *)
        BINARY_ARITH(SLASH, /)
    }

#undef BINARY_ARITH
#undef BINARY_COMP
#undef BINARY_
#undef UNARY_IDF
#undef UNARY_
}
