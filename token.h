#pragma once
#include <string>

enum class TokenType {
    LET,
    IDENT,
    NUMBER,
    ASSIGN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    PRINT,
    IF,
    ELSE,
    WHILE,
    LBRACE,
    RBRACE,
    SEMICOLON,
    END
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};