#include "Parser.hpp"
#include "Value.hpp"

#include <algorithm>

std::unordered_map<TokenType, int> Parser::binaryPrecidence;

Parser::Parser(std::vector<Token>& tokens)
    : tokens(tokens), currentToken(0), hadError(false)
{
    init();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> root;
    while (!match(TokenType::EOF_TOKEN))
    {
        try
        {
            root.push_back(declaration());
        }
        catch (const TokenError& e)
        {
            hadError = true;
            std::cout << e << std::endl;
        }
    }

    return std::move(root);
}

bool Parser::fail()
{
    return hadError;
}

void Parser::init()
{
    static bool isInit = false;
    if (!isInit)
    {
        isInit = true;

        // Bitwise
        binaryPrecidence[TokenType::BIT_OR] = 10;
        binaryPrecidence[TokenType::BIT_XOR] = 11;
        binaryPrecidence[TokenType::BIT_AND] = 12;

        // Equality
        binaryPrecidence[TokenType::EQUAL_EQUAL] = 20;
        binaryPrecidence[TokenType::BANG_EQUAL] = 20;

        // Comparison
        binaryPrecidence[TokenType::LESS] = 30;
        binaryPrecidence[TokenType::LESS_EQUAL] = 30;
        binaryPrecidence[TokenType::GREAT] = 30;
        binaryPrecidence[TokenType::GREAT_EQUAL] = 30;

        // Bitshift
        binaryPrecidence[TokenType::BITSHIFT_LEFT] = 40;
        binaryPrecidence[TokenType::BITSHIFT_RIGHT] = 40;

        // Addition
        binaryPrecidence[TokenType::PLUS] = 50;
        binaryPrecidence[TokenType::MINUS] = 50;

        // Multiplication
        binaryPrecidence[TokenType::STAR] = 60;
        binaryPrecidence[TokenType::SLASH] = 60;
        binaryPrecidence[TokenType::MODULUS] = 60;
    }
}

Token& Parser::advance()
{
    return this->tokens[this->currentToken++];
}

Token& Parser::peek()
{
    return this->tokens[this->currentToken];
}

Token& Parser::consumed()
{
    return this->tokens[this->currentToken - 1];
}

bool Parser::match(TokenType type)
{
    TokenType next = tokens[currentToken].type;
    if (next == type)
    {
        currentToken++;
        return true;
    }

    return false;
}

bool Parser::match(std::vector<TokenType> types)
{
    return std::any_of(types.begin(), types.end(), [this](TokenType& type) { return match(type); });
}

Token& Parser::consume(TokenType type, const std::string& msg)
{
    if (tokens[currentToken].type == type)
    {
        return advance();
    }
    else
    {
        throw TokenError(msg, tokens[currentToken]);
    }
}

int Parser::getPrecidence(const Token& op)
{
    using namespace std::string_literals;

    auto it = binaryPrecidence.find(op.type);
    if (it == binaryPrecidence.end())
        return -1;
    return it->second;
}

bool Parser::tryParseTypeName(std::shared_ptr<Type>& out_type)
{
    size_t start = currentToken; // anchor position
    try
    {
        out_type = parseTypeName();
        return true;
    }
    catch (const TokenError& err)
    {
        currentToken = start; // restore position
        out_type = nullptr;
        return false;
    }
}

std::shared_ptr<Type> Parser::parseTypeName()
{
    using pt = TypePrimitive::PrimitiveTag;
    Token base = advance();
    std::shared_ptr<Type> type = nullptr;
    switch (base.type)
    {
    case TokenType::VOID:
        type = std::make_unique<TypePrimitive>(pt::VOID);
        break;
    case TokenType::BOOL:
        type = std::make_unique<TypePrimitive>(pt::BOOL);
        break;
    case TokenType::CHAR:
        type = std::make_unique<TypePrimitive>(pt::CHAR);
        break;
    case TokenType::INT:
        type = std::make_unique<TypePrimitive>(pt::INT);
        break;
    case TokenType::FLOAT:
        type = std::make_unique<TypePrimitive>(pt::FLOAT);
        break;
    case TokenType::DOUBLE:
        type = std::make_unique<TypePrimitive>(pt::DOUBLE);
        break;
    case TokenType::STRING:
        type = std::make_unique<TypeString>();
        break;
    case TokenType::IDENTIFIER:
        // TODO add user defined type pls :)
        throw TokenError("User Defined types are not added yet sadly :(", base);
        break;
    default:
        throw TokenError("Invalid token for type name", base);
        break;
    }

    if (match(TokenType::OPEN_PAREN))
    {
        auto func_type = std::make_shared<TypeFunction>(std::move(type));
        if (!match(TokenType::CLOSE_PAREN))
        {
            do
            {
                func_type->param_types.push_back(parseTypeName());
            } while (match(TokenType::COMMA));
            consume(TokenType::CLOSE_PAREN, "Expect ')' after parameter types");
        }

        type = std::move(func_type);
    }
    else if (match(TokenType::BIT_AND))
    {
        // TODO owner ptr
    }
    else if (match(TokenType::STAR))
    {
        // TODO referance ptr
    }
    else if (match(TokenType::OPEN_BRACKET))
    {
        // TODO Array Types
    }

    return std::move(type);
}

std::unique_ptr<Stmt> Parser::declaration()
{
    if (match(TokenType::CLASS))
    {
        // TODO implement classes
        throw TokenError("Classes are not implemented.", consumed());
    }
    else if (match(TokenType::NAMESPACE))
    {
        // TODO implement namespaceses
        throw TokenError("Namespaced are not implemented.", consumed());
    }
    else
    {
        std::shared_ptr<Type> type = parseTypeName();
        Token name = advance();
        Token next = peek();
        switch (next.type)
        {
        case TokenType::OPEN_PAREN:
            return funcDecl(std::move(type), name);
        case TokenType::SEMI_COLON:
        case TokenType::EQUAL:
            return varDecl(std::move(type), name);
        default:
            throw TokenError("Invalid token after variable or function identifier.", next);
        }
    }
}

std::unique_ptr<Stmt> Parser::varDecl(std::shared_ptr<Type> type, Token name)
{
    std::unique_ptr<Expr> initter = nullptr;
    Token equal;
    if (match(TokenType::EQUAL))
    {
        equal = consumed();
        initter = parseExpr();
    }
    consume(TokenType::SEMI_COLON, "Expect ';' after a variable decleration.");
    if(!initter)
        equal = consumed();
    return std::make_unique<DeclVar>(name, equal, std::move(type), std::move(initter));
}

std::unique_ptr<Stmt> Parser::funcDecl(std::shared_ptr<Type> type, Token name)
{
    advance(); // '('
    std::vector<std::shared_ptr<Type>> param_types;
    std::vector<Token> param_names;
    if (!match(TokenType::CLOSE_PAREN))
    {
        do
        {
            param_types.push_back(parseTypeName());
            param_names.push_back(consume(TokenType::IDENTIFIER, "Expect an identifier for a type name."));
        } while (match(TokenType::COMMA));
        consume(TokenType::CLOSE_PAREN, "Expect ')' after function parameters.");
    }

    type = std::make_shared<TypeFunction>(type, param_types);
    consume(TokenType::OPEN_BRACE, "Expect '{' before the function body block.");

    std::unique_ptr<StmtBody> body = bodyStatement();

    return std::make_unique<DeclFunc>(name, std::move(type), std::move(param_names), std::move(body));
}

std::unique_ptr<Stmt> Parser::statement()
{
    Token next = peek();
    switch (next.type)
    {
    case TokenType::OPEN_BRACE:
        advance();
        return bodyStatement();
    case TokenType::RETURN:
        return returnStatement();
    default:
        return exprStatement();
    }
}

std::unique_ptr<StmtExpr> Parser::exprStatement()
{
    std::unique_ptr<Expr> expr = parseExpr();
    consume(TokenType::SEMI_COLON, "Expect ';' at the end of an expression statement.");
    Token semicolon = consumed();
    return std::make_unique<StmtExpr>(std::move(expr), semicolon);
}

std::unique_ptr<StmtBody> Parser::bodyStatement()
{
    std::vector<std::unique_ptr<Stmt>> body;
    while (!match(TokenType::CLOSE_BRACE))
    {
        std::shared_ptr<Type> type;
        if (tryParseTypeName(type))
        {
            body.push_back(varDecl(type, advance()));
        }
        else
        {
            body.push_back(statement());
        }
    }
    return std::make_unique<StmtBody>(std::move(body));
}

std::unique_ptr<StmtReturn> Parser::returnStatement() 
{
    Token retToken = advance();
    std::unique_ptr<Expr> retExpr = nullptr;
    if(!match(TokenType::SEMI_COLON))
    {
        retExpr = parseExpr();
        consume(TokenType::SEMI_COLON, "Expect ';' after the expression of a return statement.");
    }
    return std::make_unique<StmtReturn>(retToken, std::move(retExpr));
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment()
{
    std::unique_ptr<Expr> expr = logic_or();
    if (match(TokenType::EQUAL))
    {
        Token eq = consumed();
        std::unique_ptr<Expr> asgn = parseExpr();

        if (expr->instance == Expr::Instance::VariableGet)
        {
            Token name = dynamic_cast<ExprVariableGet*>(expr.get())->name;
            expr = std::make_unique<ExprVariableSet>(name, eq, std::move(asgn));
        }
        else
        {
            throw TokenError("Invalid assignment target.", eq);
        }
    }

    return std::move(expr);
}

std::unique_ptr<Expr> Parser::logic_or()
{
    std::unique_ptr<Expr> lhs = logic_and();
    while (match(TokenType::OR))
    {
        Token op = consumed();
        std::unique_ptr<Expr> rhs = logic_and();
        lhs = std::make_unique<ExprLogic>(std::move(lhs), std::move(rhs), op);
    }
    return std::move(lhs);
}

std::unique_ptr<Expr> Parser::logic_and()
{
    std::unique_ptr<Expr> lhs = binary(prefix(), 0);
    while (match(TokenType::AND))
    {
        Token op = consumed();
        std::unique_ptr<Expr> rhs = binary(prefix(), 0);
        lhs = std::make_unique<ExprLogic>(std::move(lhs), std::move(rhs), op);
    }
    return std::move(lhs);
}

std::unique_ptr<Expr> Parser::binary(std::unique_ptr<Expr> lhs, int precidence)
{
    int tokenPrec = getPrecidence(peek());
    while (tokenPrec >= precidence)
    {
        Token op = advance();
        std::unique_ptr<Expr> rhs = prefix();

        if (getPrecidence(peek()) > tokenPrec)
            rhs = binary(std::move(rhs), tokenPrec + 1);

        lhs = std::make_unique<ExprBinary>(std::move(lhs), std::move(rhs), op);
        tokenPrec = getPrecidence(peek());
    }
    return std::move(lhs);
}

std::unique_ptr<Expr> Parser::prefix()
{
    if (match({TokenType::MINUS, TokenType::PLUS, TokenType::MINUS_MINUS, TokenType::PLUS_PLUS}))
    {
        Token op = consumed();
        std::unique_ptr<Expr> rhs = unary();
        return std::make_unique<ExprUnary>(std::move(rhs), op);
    }

    return unary();
}

std::unique_ptr<Expr> Parser::unary()
{
    if (match({TokenType::BANG, TokenType::TILDE}))
    {
        Token op = consumed();
        std::unique_ptr<Expr> rhs = unary();
        return std::make_unique<ExprUnary>(std::move(rhs), op);
    }

    return call();
}

std::unique_ptr<Expr> Parser::call()
{
    std::unique_ptr<Expr> expr = primary();
    while (match(TokenType::OPEN_PAREN))
    {
        Token token = consumed();
        std::vector<std::unique_ptr<Expr>> args;
        if (!match(TokenType::CLOSE_PAREN))
        {
            do
            {
                args.push_back(parseExpr());
            } while (match(TokenType::COMMA));
            consume(TokenType::CLOSE_PAREN, "Expect ')' after call arguments.");
        }
        expr = std::make_unique<ExprCall>(std::move(expr), std::move(args), token);
    }
    return std::move(expr);
}

std::unique_ptr<Expr> Parser::primary()
{
    Token token = advance();

    auto make_literal = [&](const auto& val) {
        return std::make_unique<ExprLiteral>(Value(val), token);
    };

    switch (token.type)
    {
    case TokenType::TRUE:
        return make_literal(true);
    case TokenType::FALSE:
        return make_literal(false);
    case TokenType::INTEGER_LITERAL:
        return make_literal(token.getInteger());
    case TokenType::FLOAT_LITERAL:
        return make_literal(token.getFloat());
    case TokenType::CHAR_LITERAL:
        return make_literal(token.getChar());
    case TokenType::DOUBLE_LITERAL:
        return make_literal(token.getDouble());
    case TokenType::STRING_LITERAL:
        return make_literal(token.getString());
    case TokenType::OPEN_PAREN:
    {
        std::unique_ptr<Expr> inside = parseExpr();
        consume(TokenType::CLOSE_PAREN, "Expect ')' after grouping expression.");
        return std::move(inside);
    }
    case TokenType::IDENTIFIER:
        return std::make_unique<ExprVariableGet>(token);
    default:
        throw TokenError("Invalid identifier.", token);
    }
}

std::ostream& operator<<(std::ostream& os, const Parser::TokenError& err)
{
    os << "[ERROR] " << err.msg << " [line: " << err.token.line << ", col: " << err.token.col << "]\n";
    os << err.token.toStringInLine(Logger::RED);
    return os;
}
