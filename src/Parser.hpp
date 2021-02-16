#pragma once
#include "Expr.hpp"
#include "Token.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class Parser
{
public:
    Parser() = delete;
    explicit Parser(std::vector<Token>& tokens);
    ~Parser() = default;

    std::unique_ptr<Expr> parse();
    bool fail();

private:
    static void init();

    Token& advance();
    Token& peek();
    Token& consumed();
    bool match(TokenType type);
    bool match(std::vector<TokenType> types);
    void consume(TokenType type, const std::string& msg);
    int getPrecidence(const Token& op);

    std::unique_ptr<Expr> parseExpr();
    // std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logic_or();
    std::unique_ptr<Expr> logic_and();
    std::unique_ptr<Expr> binary(std::unique_ptr<Expr> lhs, int precidence);
    std::unique_ptr<Expr> prefix();
    std::unique_ptr<Expr> unary();
    // std::unique_ptr<Expr> postfix();
    // std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();

private:
    static std::unordered_map<TokenType, int> binaryPrecidence;
    std::vector<Token>& tokens;
    size_t currentToken;
    bool hadError;

public:
    class TokenError
    {
    public:
        std::string msg;
        Token token;

        TokenError() = delete;
        explicit TokenError(const std::string& msg, Token token)
            : msg(std::move(msg)), token(token)
        {}

        ~TokenError() = default;

        friend std::ostream& operator<<(std::ostream& os, const TokenError& err);
    };
};