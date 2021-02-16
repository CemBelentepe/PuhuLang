#include "Token.hpp"
#include "Log.hpp"
#include <iostream>
#include <sstream>

std::vector<std::string_view> Token::lines;

Token::Token()
    : type(TokenType::ERROR), line(-1), col(-1), lexeme("")
{
}

Token::Token(TokenType type, int line, int col, std::string_view lexeme)
    : type(type), line(line), col(col), lexeme(lexeme)
{
}

int Token::getInteger() const
{
    std::stringstream ss;
    ss << lexeme;
    int val;
    ss >> val;
    return val;
}

double Token::getDouble() const
{
    std::stringstream ss;
    ss << lexeme;
    double val;
    ss >> val;
    return val;
}

float Token::getFloat() const
{
    std::stringstream ss;
    ss << lexeme.substr(0, lexeme.size() - 1);
    float val;
    ss >> val;
    return val;
}

std::string Token::getString() const
{
    std::string val(lexeme.substr(1, lexeme.size() - 2));
    return val;
}

char Token::getChar() const
{
    std::stringstream ss;
    ss << lexeme.substr(1, 1);
    char val;
    ss >> val;
    return val;
}

void Token::showInLine(Logger::LogColor color) const
{
    std::cout << toStringInLine(color);
}

std::string Token::toStringInLine(Logger::LogColor color) const
{
    std::stringstream ss;

    std::string_view& sLine = lines[line - 1];
    ss << sLine.substr(0, col-1);
    Logger::log(ss, sLine.substr(col-1, lexeme.size()), color);
    ss << sLine.substr(col+lexeme.size()-1, sLine.size() - col - lexeme.size() + 1) << "\n";
    for (int i = 0; i < col - 1; i++)
        ss << ' ';
    Logger::setColor(ss, color);
    ss << '^';
    for (int i = 1; i < lexeme.size(); i++)
        ss << '~';
    Logger::setColor(ss, Logger::DEFAULT_COLOR);
    ss << "\n";

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Token length: " << token.lexeme.size() << ", line: " << token.line << ", col: " << token.col << ", type: "
       << (int)token.type << ", lexeme: " << token.lexeme;
    return os;
}
