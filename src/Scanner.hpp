#pragma once
#include "Token.hpp"

#include <string>
#include <vector>

class Scanner
{
public:
    Scanner() = delete;
    explicit Scanner(std::string& source);
    std::vector<Token> scanTokens();
	bool fail();

    ~Scanner() = default;

private:
    std::string& source;
    int startPosition;
    int currentPosition;
    int line;
    int col;
	bool error;

    struct TokenError
    {
        std::string msg;
        Token token;
        TokenError(const std::string& msg, Token token)
            : msg(msg), token(token)
        {
        }
    };

    Token scanToken();
    Token makeToken(TokenType type);

    Token stringLiteral();
    Token charLiteral();
    Token identifierLiteral(char start);
    Token numberLiteral();

    void skipWhitespace();

    char advance();
    bool match(char c);
    char peek();
    char peekNext();
    bool isAtEnd();

    bool isAlpha(char c);
    bool isDigit(char c);

    TokenError errorToken(const std::string& msg);
};