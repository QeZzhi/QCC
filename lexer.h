#pragma once
#include <string>
#include <vector>
#include "token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos;
    int line;

    char peek();
    char advance();
    bool isAtEnd();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& value);
    Token readNumber();
    Token readIdent();
};