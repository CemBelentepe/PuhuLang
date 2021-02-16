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

            root.push_back(statement());
        }
        catch (const TokenError& e)
        {
            hadError = true;
            std::cout << e << '\n';
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

void Parser::consume(TokenType type, const std::string& msg)
{
    if (tokens[currentToken].type == type)
    {
        advance();
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
        do
        {
            func_type->param_types.push_back(parseTypeName());
        } while (match(TokenType::COMMA));
        consume(TokenType::CLOSE_PAREN, "Expect ')' after parameter types");

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

std::unique_ptr<Stmt> Parser::statement()
{
    return exprStatement();
}

std::unique_ptr<Stmt> Parser::exprStatement()
{
    std::unique_ptr<Expr> expr = parseExpr();
    consume(TokenType::SEMI_COLON, "Expect ';' at the end of an expression statement.");
    Token semicolon = consumed();
    return std::make_unique<StmtExpr>(std::move(expr), semicolon);
}

std::unique_ptr<Expr> Parser::parseExpr()
{
    return logic_or();
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

    return primary();
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
    // case TokenType::IDENTIFIER:
    //     return std::make_unique<ExprVariableGet>(token);
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
