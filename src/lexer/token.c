#include "lexer/token.h"

#include <stdlib.h>

const char* token_type_name(TokenType type) {
    switch (type) {
        case TOK_LET:       return "'let'";
        case TOK_IDENT:     return "identifier";
        case TOK_NUMBER:    return "number";
        case TOK_ASSIGN:    return "'='";
        case TOK_PLUS:      return "'+'";
        case TOK_MINUS:     return "'-'";
        case TOK_STAR:      return "'*'";
        case TOK_SLASH:     return "'/'";
        case TOK_LPAREN:    return "'('";
        case TOK_RPAREN:    return "')'";
        case TOK_PRINT:     return "'print'";
        case TOK_IF:        return "'if'";
        case TOK_ELSE:      return "'else'";
        case TOK_WHILE:     return "'while'";
        case TOK_LBRACE:    return "'{'";
        case TOK_RBRACE:    return "'}'";
        case TOK_SEMICOLON: return "';'";
        case TOK_END:       return "end of file";
    }
    return "unknown token";
}

void token_free(Token* token) {
    if (!token) return;
    free(token->value);
    token->value = NULL;
}

void tokens_free(Token* tokens, int count) {
    if (!tokens) return;
    for (int i = 0; i < count; i++) {
        token_free(&tokens[i]);
    }
    free(tokens);
}
