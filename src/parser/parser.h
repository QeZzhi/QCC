#ifndef QCC_PARSER_H
#define QCC_PARSER_H

#include "ast/ast.h"
#include "lexer/token.h"

typedef struct {
    Token* toks;
    int count;
    int pos;
    int had_error;
} Parser;

Parser* parser_create(Token* tokens, int count);
void parser_destroy(Parser* parser);

/* Возвращает NULL при синтаксической ошибке: сообщение уже напечатано
   в stderr, процесс не завершается. */
BlockNode* parser_parse(Parser* parser);

#endif
