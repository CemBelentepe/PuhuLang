#include "Scanner.hpp"
#include <iostream>
#include <sstream>
#include <string_view>

Scanner::Scanner(std::string& source)
    : source(source), startPosition(0), currentPosition(0), line(1), col(1), error(false)
{
    Token::lines.push_back(std::string_view(source));
}

std::vector<Token> Scanner::scanTokens()
{
    std::vector<Token> tokens;
    std::vector<TokenError> errors;
    
    do
    {
        try
        {
            tokens.push_back(scanToken());
        }
        catch (const TokenError& err)
        {
            error = true;
            tokens.push_back(err.token);
            errors.push_back(err);
        }
    } while (tokens.back().type != TokenType::EOF_TOKEN);

    for(const auto& err : errors)
    {
        std::cout << err.msg;
        err.token.showInLine();
    }

    return tokens;
}

bool Scanner::fail() 
{
    return error;
}

Token Scanner::scanToken()
{
    skipWhitespace();

    this->startPosition = currentPosition;
    if (this->isAtEnd())
        return makeToken(TokenType::EOF_TOKEN);

    char c = advance();
    switch (c)
    {
    case '{':
        return makeToken(TokenType::OPEN_BRACE);
    case '}':
        return makeToken(TokenType::CLOSE_BRACE);
    case '[':
        return makeToken(TokenType::OPEN_BRACKET);
    case ']':
        return makeToken(TokenType::CLOSE_BRACKET);
    case '(':
        return makeToken(TokenType::OPEN_PAREN);
    case ')':
        return makeToken(TokenType::CLOSE_PAREN);
    case '<':
        if (match('='))
            return makeToken(TokenType::LESS_EQUAL);
        if (match('<'))
        {
            if (match('='))
                return makeToken(TokenType::BITSHIFT_LEFT_EQUAL);
            else
                return makeToken(TokenType::BITSHIFT_LEFT);
        }
        return makeToken(TokenType::LESS);
    case '>':
        if (match('='))
            return makeToken(TokenType::GREAT_EQUAL);
        if (match('>'))
        {
            if (match('='))
                return makeToken(TokenType::BITSHIFT_RIGHT_EQUAL);
            else
                return makeToken(TokenType::BITSHIFT_RIGHT);
        }
        return makeToken(TokenType::GREAT);
    case '.':
        return makeToken(TokenType::DOT);
    case ',':
        return makeToken(TokenType::COMMA);
    case ';':
        return makeToken(TokenType::SEMI_COLON);
    case ':':
        if (match(':'))
            return makeToken(TokenType::DOUBLE_COLON);
        return makeToken(TokenType::COLON);
    case '*':
        if (match('='))
            return makeToken(TokenType::STAR_EQUAL);
        return makeToken(TokenType::STAR);
    case '+':
        if (match('+'))
            return makeToken(TokenType::PLUS_PLUS);
        if (match('='))
            return makeToken(TokenType::PLUS_EQUAL);
        return makeToken(TokenType::PLUS);
    case '-':
        if (match('-'))
            return makeToken(TokenType::MINUS_MINUS);
        if (match('='))
            return makeToken(TokenType::MINUS_EQUAL);
        if (match('>'))
            return makeToken(TokenType::ARROW);
        return makeToken(TokenType::MINUS);
    case '/':
        if (match('='))
            return makeToken(TokenType::SLASH_EQUAL);
        return makeToken(TokenType::SLASH);
    case '%':
        return makeToken(TokenType::MODULUS);
    case '!':
        if (match('='))
            return makeToken(TokenType::BANG_EQUAL);
        return makeToken(TokenType::BANG);
    case '=':
        if (match('='))
            return makeToken(TokenType::EQUAL_EQUAL);
        return makeToken(TokenType::EQUAL);
    case '&':
        if (match('='))
            return makeToken(TokenType::BIT_AND_EQUAL);
        if (match('&'))
            return makeToken(TokenType::AND);
        return makeToken(TokenType::BIT_AND);
    case '|':
        if (match('='))
            return makeToken(TokenType::BIT_OR_EQUAL);
        if (match('|'))
            return makeToken(TokenType::OR);
        return makeToken(TokenType::BIT_OR);
    case '^':
        if (match('='))
            return makeToken(TokenType::BIT_XOR_EQUAL);
        return makeToken(TokenType::BIT_XOR);
    case '~':
        return makeToken(TokenType::TILDE);

    case '"':
        return stringLiteral();
    case '\'':
        return charLiteral();

    default:
        if (this->isAlpha(c))
            return identifierLiteral(c);
        else if (this->isDigit(c))
            return numberLiteral();
        throw errorToken("Unexpected character");
    }
}

Token Scanner::makeToken(TokenType type)
{
    size_t len = currentPosition - startPosition;
    std::string_view lexeme(source);
    lexeme = lexeme.substr(startPosition, len);
    return Token(type, line, col - len, lexeme);
}

Token Scanner::stringLiteral()
{
    while (peek() != '"' && !this->isAtEnd())
    {
        advance();
    }

    if (isAtEnd())
        throw errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::STRING_LITERAL);
}

Token Scanner::charLiteral()
{
    while (peek() != '\'' && !this->isAtEnd())
    {
        advance();
    }

    if (isAtEnd())
        throw errorToken("Unterminated char.");

    advance();

    return makeToken(TokenType::CHAR_LITERAL);
}

Token Scanner::identifierLiteral(char start)
{
    std::stringstream lexeme;
    lexeme << start;
    while ((this->isAlpha(peek()) || this->isDigit(peek())))
    {
        lexeme << advance();
    }

    if (lexeme.str() == "using")
        return makeToken(TokenType::USING);
    if (lexeme.str() == "namespace")
        return makeToken(TokenType::NAMESPACE);
    if (lexeme.str() == "class")
        return makeToken(TokenType::CLASS);
    if (lexeme.str() == "var")
        return makeToken(TokenType::VAR);
    if (lexeme.str() == "if")
        return makeToken(TokenType::IF);
    if (lexeme.str() == "else")
        return makeToken(TokenType::ELSE);
    if (lexeme.str() == "int")
        return makeToken(TokenType::INT);
    if (lexeme.str() == "float")
        return makeToken(TokenType::FLOAT);
    if (lexeme.str() == "double")
        return makeToken(TokenType::DOUBLE);
    if (lexeme.str() == "char")
        return makeToken(TokenType::CHAR);
    if (lexeme.str() == "string")
        return makeToken(TokenType::STRING);
    if (lexeme.str() == "bool")
        return makeToken(TokenType::BOOL);
    if (lexeme.str() == "void")
        return makeToken(TokenType::VOID);
    if (lexeme.str() == "while")
        return makeToken(TokenType::WHILE);
    if (lexeme.str() == "for")
        return makeToken(TokenType::FOR);
    if (lexeme.str() == "true")
        return makeToken(TokenType::TRUE);
    if (lexeme.str() == "false")
        return makeToken(TokenType::FALSE);
    if (lexeme.str() == "null")
        return makeToken(TokenType::NULL_TOKEN);
    if (lexeme.str() == "return")
        return makeToken(TokenType::RETURN);
    if (lexeme.str() == "heap")
        return makeToken(TokenType::HEAP);
    if (lexeme.str() == "ref")
        return makeToken(TokenType::REF);
    if (lexeme.str() == "take")
        return makeToken(TokenType::TAKE);

    return makeToken(TokenType::IDENTIFIER);
}

Token Scanner::numberLiteral()
{
    while (this->isDigit(peek()))
    {
        advance();
    }
    if (match('.'))
    {
        while (this->isDigit(peek()))
        {
            advance();
        }
        if (match('f') || match('F'))
            return makeToken(TokenType::FLOAT_LITERAL);
        else
            return makeToken(TokenType::DOUBLE_LITERAL);
    }
    else if (match('f') || match('F'))
        return makeToken(TokenType::FLOAT_LITERAL);
    return makeToken(TokenType::INTEGER_LITERAL);
}

void Scanner::skipWhitespace()
{
    while (true)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
            advance();
            break;
        case '\n':
            Token::lines.back() = Token::lines.back().substr(0, this->col-1);
            this->line++;
            this->col = 0;
            advance();
            Token::lines.push_back(std::string_view(source).substr(currentPosition));
            break;

        case '/':
            if (peekNext() == '/')
            {
                while (peek() != '\n' && !this->isAtEnd())
                    advance();
                skipWhitespace();
            }
            else if (peekNext() == '*')
            {
                while (!(peek() == '*' && peekNext() == '/') && !this->isAtEnd())
                {
                    char x = advance();
                    if (x == '\n')
                        this->line++;
                }
                this->currentPosition += 2;
                skipWhitespace();
            }
            return;
        default:
            return;
        }
    }
}

char Scanner::advance()
{
    this->col++;
    return source[this->currentPosition++];
}

bool Scanner::match(char c)
{
    if (this->isAtEnd() || source[this->currentPosition] != c)
        return false;

    this->col++;
    this->currentPosition++;
    return true;
}

char Scanner::peek()
{
    return source[this->currentPosition];
}

char Scanner::peekNext()
{
    if (this->isAtEnd())
        return '\0';
    return source[this->currentPosition + 1];
}

bool Scanner::isAtEnd()
{
    return this->currentPosition == source.length();
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

Scanner::TokenError Scanner::errorToken(const std::string& msg)
{
    std::stringstream ss;
    ss << "[ERROR] " << msg << " [line: " << this->line << ", col: " << this->col << "]\n";
    return TokenError(ss.str(), makeToken(TokenType::ERROR));
}
