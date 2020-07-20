#include "Parser.h"
#include "AST.h"
#include "Natives.hpp"
#include "Value.hpp"

Parser::Parser(std::vector<Token>& tokens)
    : tokens(tokens), currentToken(0), depth(0)
{
    globals.insert({"print", new NativeFunc(native_print, TypeTag::VOID, {Type(TypeTag::STRING)}, true)});
    globals.insert({"input", new NativeFunc(native_input, TypeTag::STRING, {})});
    globals.insert({"clock", new NativeFunc(native_clock, TypeTag::DOUBLE, {})});
    globals.insert({"inputInt", new NativeFunc(native_inputInt, TypeTag::INTEGER, {})});
}

std::vector<Stmt*> Parser::parse()
{
    this->currentToken = 0;

    std::vector<Stmt*> root;
    while (!match(TokenType::EOF_TOKEN))
    {
        root.push_back(decleration());
    }

    return root;
}

Type Parser::parseTypeName()
{
    Type type;
    TypeTag val = getDataType(peek());
    int i = 0;
    do
    {
        if (i > 0)
            type.intrinsicType = std::make_shared<Type>(type);
        type.tag = val;
        if (type.isPrimitive() && type.intrinsicType != nullptr && type.intrinsicType->isPrimitive())
        {
            error("Invalid type name.");
            break;
        }

        advance();
        i++;
    } while ((val = getDataType(peek())) != TypeTag::ERROR);

    return type;
}

bool Parser::isTypeName(Token& token)
{
    switch (token.type)
    {
    case TokenType::INT:
    case TokenType::FLOAT:
    case TokenType::DOUBLE:
    case TokenType::CHAR:
    case TokenType::STRING:
    case TokenType::BOOL:
    case TokenType::VOID:
        return true;
    default:
        return false;
    }
}

TypeTag Parser::getDataType(Token& token)
{
    switch (token.type)
    {
    case TokenType::INT:
        return TypeTag::INTEGER;
    case TokenType::FLOAT:
        return TypeTag::FLOAT;
    case TokenType::DOUBLE:
        return TypeTag::DOUBLE;
    case TokenType::CHAR:
        return TypeTag::CHAR;
    case TokenType::STRING:
        return TypeTag::STRING;
    case TokenType::BOOL:
        return TypeTag::BOOL;
    case TokenType::VOID:
        return TypeTag::VOID;
    case TokenType::STAR:
        return TypeTag::POINTER;
    case TokenType::OPEN_BRACKET:
        if (peekNext().type == TokenType::CLOSE_BRACKET)
        {
            advance();
            return TypeTag::ARRAY;
        }
        else
            return TypeTag::ERROR;
    default:
        return TypeTag::ERROR;
    }
}

void Parser::consume(TokenType type, const char* message)
{
    if (tokens[currentToken].type == type)
    {
        advance();
    }
    else
    {
        errorAtToken(message);
    }
}

void Parser::consumeNext(TokenType type, const char* message)
{
    if (tokens[currentToken + 1].type == type)
    {
        advance();
    }
    else
    {
        errorAtToken(message);
    }
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

bool Parser::matchCast()
{
    return tokens[currentToken].type == TokenType::OPEN_PAREN && getCast() != TypeTag::ERROR && tokens[currentToken + 2].type == TokenType::CLOSE_PAREN;
}

Type Parser::getCast()
{
    switch (tokens[currentToken + 1].type)
    {
    case TokenType::INT:
        return TypeTag::INTEGER;
    case TokenType::FLOAT:
        return TypeTag::FLOAT;
    case TokenType::DOUBLE:
        return TypeTag::DOUBLE;
    case TokenType::CHAR:
        return TypeTag::CHAR;
    case TokenType::STRING:
        return TypeTag::STRING;
    case TokenType::BOOL:
        return TypeTag::BOOL;
    case TokenType::VOID:
        return TypeTag::VOID;
    default:
        return TypeTag::ERROR;
    }
}

inline Expr* Parser::typeError(const char* message) const
{
    std::cout << "At " << tokens[currentToken - 1] << message << std::endl;
    return nullptr;
}

inline void Parser::error(const char* message)
{
    std::cout << message << std::endl;
    this->panic();
}

inline void Parser::errorAtToken(const char* message)
{
    std::cout << "[line" << tokens[currentToken].line << "] Error" << message << std::endl;
    this->panic();
}

void Parser::panic()
{
    TokenType type = tokens[currentToken].type;
    while (type != TokenType::EOF_TOKEN)
    {
        if (type == TokenType::SEMI_COLON)
        {
            advance();
            return;
        }

        switch (type)
        {
        case TokenType::USING:
        case TokenType::NAMESPACE:
        case TokenType::CLASS:
        case TokenType::IF:
        case TokenType::INT:
        case TokenType::FLOAT:
        case TokenType::DOUBLE:
        case TokenType::CHAR:
        case TokenType::STRING:
        case TokenType::BOOL:
        case TokenType::VOID:
        case TokenType::WHILE:
        case TokenType::FOR:
        case TokenType::RETURN:
            return;

        case TokenType::IDENTIFIER:
            // TODO: add user defined types
            // if (isTypeName(tokens[currentToken]))
            return;

        default:
            break;
        }

        type = advance().type;
    }
}

Stmt* Parser::decleration()
{
    TokenType token = peek().type;
    if (token == TokenType::CLASS)
    {
        // TODO parse a class
    }
    else if (token == TokenType::NAMESPACE)
    {
        // TODO add namespaces
    }
    else
    {
        Type typeName = parseTypeName();
        if (peekNext().type == TokenType::OPEN_PAREN)
        {
            return functionDecleration(typeName);
        }
        else
        {
            return variableDecleration(typeName);
        }
    }

    return nullptr;
}

Stmt* Parser::variableDecleration(Type type)
{
    Token varName = advance();
    Expr* init = nullptr;
    if (match(TokenType::EQUAL))
        init = parseExpression();

    consume(TokenType::SEMI_COLON, "Expect ';' after variable decleration.");

    if (depth == 0)
    {
        globals.insert({varName.getString(), new Value(type)});
    }

    return new StmtVarDecleration(type, varName, init);
}

Stmt* Parser::functionDecleration(Type type)
{
    Token funcName = advance();
    std::vector<std::pair<Token, Type>> params;
    size_t totalSize = 0;

    this->depth++;
    advance(); // Open Paren (

    if (peek().type != TokenType::CLOSE_PAREN)
    {
        Type type = parseTypeName();
        Token name = advance();
        params.push_back(std::make_pair(name, type));
        totalSize += type.getSize();
    }
    while (!match(TokenType::CLOSE_PAREN))
    {
        consume(TokenType::COMMA, "Expect ',' between parameters."); // consume ','
        Type type = parseTypeName();
        Token name = advance();
        params.push_back(std::make_pair(name, type));
        totalSize += type.getSize();
    }

    consume(TokenType::OPEN_BRACE, "Expect '{' at function start."); // Opening {
    StmtBlock* body = (StmtBlock*)block();

    this->depth--;
    globals.insert(std::make_pair(funcName.getString(), new FuncValue(type, params)));

    return new StmtFunc(funcName, body, type, params);
}

Stmt* Parser::statement()
{
    TokenType token = peek().type;
    if (match(TokenType::OPEN_BRACE))
    {
        return block();
    }
    else if (token == TokenType::IF)
    {
        return ifStatement();
    }
    // else if (token == TokenType::WHILE)
    // {
    // 	whileStatement();
    // }
    // else if (token == TokenType::FOR)
    // {
    // 	forStatement();
    // }
    else if (token == TokenType::RETURN)
    {
        advance();
        Expr* ret = parseExpression();
        consume(TokenType::SEMI_COLON, "Expect ';' after a return statement.");
        return new StmtReturn(ret);
    }
    else
    {
        Expr* expr = parseExpression();
        consume(TokenType::SEMI_COLON, "Expect ';' after an expression statement.");
        return new StmtExpr(expr);
    }
}

Stmt* Parser::block()
{
    this->depth++;

    std::vector<Stmt*> stmts;
    while (peek().type != TokenType::CLOSE_BRACE && peek().type != TokenType::EOF_TOKEN)
    {
        if (isTypeName(peek()))
            stmts.push_back(variableDecleration(parseTypeName()));
        else
            stmts.push_back(statement());
    }
    consume(TokenType::CLOSE_BRACE, "Expect '}' after a block.");

    this->depth--;
    return new StmtBlock(stmts);
}

Stmt* Parser::ifStatement()
{
    advance();
    consume(TokenType::OPEN_PAREN, "Expect '(' after if keyword.");
    Token paren = consumed();
    Expr* condition = parseExpression();
    consume(TokenType::CLOSE_PAREN, "Expect ')' after if condition.");
    Stmt* then = statement();
    Stmt* els = nullptr;
    if (match(TokenType::ELSE))
        els = statement();
    return new StmtIf(condition, then, els, paren);
}

Expr* Parser::parseExpression()
{
    return assignment();
}

Expr* Parser::assignment()
{
    Expr* expr = bit_or();

    if (match(TokenType::EQUAL))
    {
        Expr* asgn = parseExpression();

        if (expr->instance == ExprType::Variable)
        {
            Token name = ((ExprVariable*)expr)->name;
            return new ExprAssignment(name, asgn);
        }

        error("Invalid assignment target.");
    }

    return expr;
}

Expr* Parser::bit_or()
{
    Expr* left = bit_xor();
    while (match(TokenType::BIT_OR))
    {
        Token op = consumed();
        Expr* right = bit_xor();
        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::bit_xor()
{
    Expr* left = bit_and();
    while (match(TokenType::BIT_XOR))
    {
        Token op = consumed();
        Expr* right = bit_and();
        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::bit_and()
{
    Expr* left = equality();
    while (match(TokenType::BIT_AND))
    {
        Token op = consumed();
        Expr* right = equality();
        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::equality()
{
    Expr* left = comparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL))
    {
        Token op = consumed();
        Expr* right = comparison();

        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::comparison()
{
    Expr* left = bitshift();
    while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) || match(TokenType::GREAT) || match(TokenType::GREAT_EQUAL))
    {
        Token op = consumed();
        Expr* right = bitshift();

        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::bitshift()
{
    Expr* left = addition();
    while (match(TokenType::BITSHIFT_LEFT) || match(TokenType::BITSHIFT_RIGHT))
    {
        Token op = consumed();
        Expr* right = addition();
        left = new ExprBinary(left, right, op);
    }
    return left;
}

Expr* Parser::addition()
{
    Expr* left = multiplication();
    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {
        Token op = consumed();
        Expr* right = multiplication();

        left = new ExprBinary(left, right, op);
    }

    return left;
}

Expr* Parser::multiplication()
{
    Expr* left = prefix();
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MODULUS))
    {
        Token op = consumed();
        Expr* right = prefix();

        left = new ExprBinary(left, right, op);
    }

    return left;
}

Expr* Parser::prefix()
{
    if (match(TokenType::MINUS) || match(TokenType::PLUS) || match(TokenType::MINUS_MINUS) || match(TokenType::PLUS_PLUS))
    {
        Token op = consumed();
        Expr* expr = unary();
        return new ExprUnary(expr, op);
    }
    else
        return unary();
}

Expr* Parser::unary()
{
    if (match(TokenType::BANG) || match(TokenType::TILDE))
    {
        Token op = consumed();
        Expr* expr = unary();
        return new ExprUnary(expr, op);
    }
    else if (matchCast())
    {
        Type type = getCast();
        this->currentToken += 3;
        Expr* expr = unary();
        return new ExprCast(type, expr);
    }
    else
        return postfix();
}

Expr* Parser::postfix()
{
    // TODO: Wire it
    return call();
}

Expr* Parser::call()
{
    Expr* expr = primary();

    if (match(TokenType::OPEN_PAREN))
    {
        Token openParen = consumed();
        std::vector<Expr*> args;
        if (peek().type != TokenType::CLOSE_PAREN)
        {
            do
            {
                args.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }
        expr = new ExprCall(expr, args, openParen);
        consume(TokenType::CLOSE_PAREN, "Expect ')' after arguments.");
    }

    return expr;
}

Expr* Parser::primary()
{
    Token token = advance();

    switch (token.type)
    {
    case TokenType::TRUE:
        return new ExprLiteral(new Value(true));
    case TokenType::FALSE:
        return new ExprLiteral(new Value(false));
    case TokenType::INTEGER_LITERAL:
        return new ExprLiteral(new Value(token.getInteger()));
    case TokenType::DOUBLE_LITERAL:
        return new ExprLiteral(new Value(token.getDouble()));
    case TokenType::FLOAT_LITERAL:
        return new ExprLiteral(new Value(token.getFloat()));
    case TokenType::STRING_LITERAL:
        return new ExprLiteral(new Value(token.getString()));
    case TokenType::OPEN_PAREN:
    {
        Expr* a = parseExpression();
        if (match(TokenType::CLOSE_PAREN))
            return a;
        else
            return typeError("Expect ')' after a grouping expression.");
    }

    case TokenType::IDENTIFIER:
        return new ExprVariable(token);

    default:
        std::cout << "[Error]Invalid identifier: " << token.getString();
        return nullptr;
    }
}