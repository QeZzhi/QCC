#pragma once
#include <vector>
#include <memory>
#include "token.h"
#include "ast.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<BlockNode> parse();

private:
    std::vector<Token> toks;
    size_t pos;

    Token peek();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    Token expect(TokenType type);

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();
};