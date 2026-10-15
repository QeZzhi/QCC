#ifndef QCC_TOKEN_H
#define QCC_TOKEN_H

typedef enum {
    TOK_LET,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_PRINT,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_END
} TokenType;

typedef struct {
    TokenType type;
    char* value; /* владеемая копия лексемы, может быть NULL */
    int line;
} Token;

/* Имя типа токена для сообщений об ошибках ("';'", "identifier", ...). */
const char* token_type_name(TokenType type);

void token_free(Token* token);
void tokens_free(Token* tokens, int count);

#endif
