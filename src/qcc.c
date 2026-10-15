#include "qcc.h"

#include "codegen/codegen.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

Bytecode* qcc_compile(const char* source) {
    Lexer* lexer = lexer_create(source);

    int token_count = 0;
    Token* tokens = lexer_tokenize(lexer, &token_count);

    if (lexer->had_error) {
        tokens_free(tokens, token_count);
        lexer_destroy(lexer);
        return NULL;
    }

    Parser* parser = parser_create(tokens, token_count);
    BlockNode* ast = parser_parse(parser);

    Bytecode* bc = ast ? codegen_generate(ast) : NULL;

    /* AST хранит собственные копии строк, поэтому токены и лексер
       можно освобождать сразу после разбора. */
    ast_free((ASTNode*)ast);
    parser_destroy(parser);
    tokens_free(tokens, token_count);
    lexer_destroy(lexer);

    return bc;
}
