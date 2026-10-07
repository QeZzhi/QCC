#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : src(source), pos(0), line(1) {}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return src[pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = src[pos++];
    if (c == '\n') line++;
    return c;
}

bool Lexer::isAtEnd() {
    return pos >= src.size();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\n' || peek() == '\r')) {
        advance();
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token{type, value, line};
}

Token Lexer::readNumber() {
    std::string num;
    while (!isAtEnd() && isdigit(peek())) {
        num += advance();
    }
    return makeToken(TokenType::NUMBER, num);
}

Token Lexer::readIdent() {
    std::string id;
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        id += advance();
    }

    if (id == "let")   return makeToken(TokenType::LET, id);
    if (id == "print") return makeToken(TokenType::PRINT, id);
    if (id == "if")    return makeToken(TokenType::IF, id);
    if (id == "else")  return makeToken(TokenType::ELSE, id);
    if (id == "while") return makeToken(TokenType::WHILE, id);

    return makeToken(TokenType::IDENT, id);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;

        char c = peek();

        if (isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }

        if (isalpha(c) || c == '_') {
            tokens.push_back(readIdent());
            continue;
        }

        switch (c) {
            case '=': advance(); tokens.push_back(makeToken(TokenType::ASSIGN, "=")); break;
            case '+': advance(); tokens.push_back(makeToken(TokenType::PLUS, "+")); break;
            case '-': advance(); tokens.push_back(makeToken(TokenType::MINUS, "-")); break;
            case '*': advance(); tokens.push_back(makeToken(TokenType::STAR, "*")); break;
            case '/': advance(); tokens.push_back(makeToken(TokenType::SLASH, "/")); break;
            case '(': advance(); tokens.push_back(makeToken(TokenType::LPAREN, "(")); break;
            case ')': advance(); tokens.push_back(makeToken(TokenType::RPAREN, ")")); break;
            case '{': advance(); tokens.push_back(makeToken(TokenType::LBRACE, "{")); break;
            case '}': advance(); tokens.push_back(makeToken(TokenType::RBRACE, "}")); break;
            case ';': advance(); tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
            default:  advance(); break;
        }
    }

    tokens.push_back(makeToken(TokenType::END, ""));
    return tokens;
}