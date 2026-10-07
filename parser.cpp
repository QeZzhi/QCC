#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : toks(tokens), pos(0) {}

Token Parser::peek() {
    return toks[pos];
}

Token Parser::advance() {
    return toks[pos++];
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type) {
    if (check(type)) return advance();
    throw std::runtime_error("Parser: unexpected token");
}

std::unique_ptr<BlockNode> Parser::parse() {
    std::unique_ptr<BlockNode> block(new BlockNode());

    while (!check(TokenType::END)) {
        block->statements.push_back(parseStatement());
    }

    return block;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::LET)) {
        Token name = expect(TokenType::IDENT);
        expect(TokenType::ASSIGN);
        auto value = parseExpression();
        expect(TokenType::SEMICOLON);

        std::unique_ptr<LetNode> node(new LetNode());
        node->name = name.value;
        node->value = std::move(value);
        return node;
    }

    if (match(TokenType::PRINT)) {
        auto value = parseExpression();
        expect(TokenType::SEMICOLON);

        std::unique_ptr<PrintNode> node(new PrintNode());
        node->value = std::move(value);
        return node;
    }

    auto expr = parseExpression();
    expect(TokenType::SEMICOLON);
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        std::string op = advance().value;
        auto right = parseFactor();

        std::unique_ptr<BinaryOpNode> node(new BinaryOpNode());
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    auto left = parsePrimary();

    while (check(TokenType::STAR) || check(TokenType::SLASH)) {
        std::string op = advance().value;
        auto right = parsePrimary();

        std::unique_ptr<BinaryOpNode> node(new BinaryOpNode());
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        std::unique_ptr<NumberNode> node(new NumberNode());
        node->value = std::stoi(toks[pos - 1].value);
        return node;
    }

    if (match(TokenType::IDENT)) {
        std::unique_ptr<VarNode> node(new VarNode());
        node->name = toks[pos - 1].value;
        return node;
    }

    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN);
        return expr;
    }

    throw std::runtime_error("Parser: expected expression");
}