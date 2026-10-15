#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/xalloc.h"

Parser* parser_create(Token* tokens, int count) {
    Parser* p = xmalloc(sizeof(Parser));
    p->toks = tokens;
    p->count = count;
    p->pos = 0;
    p->had_error = 0;
    return p;
}

void parser_destroy(Parser* p) { free(p); }

/* Зажим на случай pos == count: последний токен всегда TOK_END, поэтому
   чтения за границей массива не происходит. */
static Token peek(const Parser* p) {
    if (p->pos >= p->count) return p->toks[p->count - 1];
    return p->toks[p->pos];
}

static Token previous(const Parser* p) { return p->toks[p->pos - 1]; }

static Token advance(Parser* p) {
    Token token = peek(p);
    if (p->pos < p->count) p->pos++;
    return token;
}

static int check(const Parser* p, TokenType type) { return peek(p).type == type; }

static int match(Parser* p, TokenType type) {
    if (!check(p, type)) return 0;
    advance(p);
    return 1;
}

/* Печатаем только первую ошибку: после неё дерево разбора недостоверно,
   и лавина вторичных сообщений только мешает. */
static void parser_error(Parser* p, int line, const char* message) {
    if (!p->had_error) {
        fprintf(stderr, "error:%d: %s\n", line, message);
    }
    p->had_error = 1;
}

static void parser_error_expected(Parser* p, TokenType expected) {
    char message[128];
    snprintf(message, sizeof(message), "expected %s but found %s",
             token_type_name(expected), token_type_name(peek(p).type));
    parser_error(p, peek(p).line, message);
}

/* 1 — токен нужного типа был и он поглощён; 0 — ошибка уже напечатана. */
static int expect(Parser* p, TokenType type, Token* out) {
    if (!check(p, type)) {
        parser_error_expected(p, type);
        return 0;
    }
    Token token = advance(p);
    if (out) *out = token;
    return 1;
}

static ASTNode* parse_expression(Parser* p);
static ASTNode* parse_term(Parser* p);
static ASTNode* parse_factor(Parser* p);
static ASTNode* parse_primary(Parser* p);

static ASTNode* parse_binary_right(Parser* p, ASTNode* left, ASTNode* (*operand)(Parser*)) {
    char* op = advance(p).value; /* ast_binary_create копирует строку сразу */
    ASTNode* right = operand(p);
    if (!right) {
        ast_free(left);
        return NULL;
    }
    return ast_binary_create(op, left, right);
}

/* expression := term
   (сравнения и логика появятся здесь, когда для них будут опкоды) */
static ASTNode* parse_expression(Parser* p) { return parse_term(p); }

/* term := factor (('+' | '-') factor)* */
static ASTNode* parse_term(Parser* p) {
    ASTNode* left = parse_factor(p);
    if (!left) return NULL;

    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        ASTNode* node = parse_binary_right(p, left, parse_factor);
        if (!node) return NULL;
        left = node;
    }
    return left;
}

/* factor := primary (('*' | '/') primary)* */
static ASTNode* parse_factor(Parser* p) {
    ASTNode* left = parse_primary(p);
    if (!left) return NULL;

    while (check(p, TOK_STAR) || check(p, TOK_SLASH)) {
        ASTNode* node = parse_binary_right(p, left, parse_primary);
        if (!node) return NULL;
        left = node;
    }
    return left;
}

static ASTNode* parse_primary(Parser* p) {
    if (match(p, TOK_NUMBER)) {
        return ast_number_create(atoi(previous(p).value));
    }

    if (match(p, TOK_IDENT)) {
        return ast_var_create(previous(p).value);
    }

    if (match(p, TOK_LPAREN)) {
        ASTNode* expr = parse_expression(p);
        if (!expr) return NULL;
        if (!expect(p, TOK_RPAREN, NULL)) {
            ast_free(expr);
            return NULL;
        }
        return expr;
    }

    char message[128];
    snprintf(message, sizeof(message), "expected expression but found %s",
             token_type_name(peek(p).type));
    parser_error(p, peek(p).line, message);
    return NULL;
}

/* statement := 'let' IDENT '=' expression ';'
              | 'print' expression ';'
              | expression ';' */
static ASTNode* parse_statement(Parser* p) {
    if (match(p, TOK_LET)) {
        Token name;
        if (!expect(p, TOK_IDENT, &name)) return NULL;
        if (!expect(p, TOK_ASSIGN, NULL)) return NULL;

        ASTNode* value = parse_expression(p);
        if (!value) return NULL;
        if (!expect(p, TOK_SEMICOLON, NULL)) {
            ast_free(value);
            return NULL;
        }
        return ast_let_create(name.value, value);
    }

    if (match(p, TOK_PRINT)) {
        ASTNode* value = parse_expression(p);
        if (!value) return NULL;
        if (!expect(p, TOK_SEMICOLON, NULL)) {
            ast_free(value);
            return NULL;
        }
        return ast_print_create(value);
    }

    ASTNode* expr = parse_expression(p);
    if (!expr) return NULL;
    if (!expect(p, TOK_SEMICOLON, NULL)) {
        ast_free(expr);
        return NULL;
    }
    return expr;
}

BlockNode* parser_parse(Parser* p) {
    BlockNode* block = ast_block_create();

    while (!check(p, TOK_END)) {
        ASTNode* stmt = parse_statement(p);
        if (!stmt) {
            ast_free((ASTNode*)block);
            return NULL;
        }
        ast_block_add(block, stmt);
    }

    return block;
}
