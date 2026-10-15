#ifndef QCC_LEXER_H
#define QCC_LEXER_H

#include <stddef.h>

#include "lexer/token.h"

typedef struct {
    char* src;  /* владеемая копия исходника */
    size_t len; /* длина src, считается один раз при создании */
    size_t pos;
    int line;
    int had_error; /* 1, если лексер уже сообщил об ошибке */
} Lexer;

Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);

/* Возвращает массив токенов, всегда заканчивающийся TOK_END.
   Сообщения об ошибках печатаются в stderr, признак — lexer->had_error. */
Token* lexer_tokenize(Lexer* lexer, int* out_count);

#endif
