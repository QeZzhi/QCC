// ============================================================
// qcc.c — главная функция компилятора QCC
// Собирает всю цепочку: лексер → парсер → codegen.
// qcc.c — main function of QCC compiler
// Assembles the whole pipeline: lexer → parser → codegen.
// ============================================================

#include "qcc.h" // Подключение заголовка QCC | Connecting QCC header

#include "codegen/codegen.h" // codegen_generate | codegen_generate
#include "lexer/lexer.h"     // lexer_create, lexer_tokenize, lexer_destroy | lexer_create, lexer_tokenize, lexer_destroy
#include "parser/parser.h"   // parser_create, parser_parse, parser_destroy | parser_create, parser_parse, parser_destroy

// ------------------------------------------------------------
// qcc_compile — скомпилировать исходник Qora в байт-код
// Принимает строку с исходником, возвращает байт-код или NULL
// ------------------------------------------------------------
Bytecode* qcc_compile(const char* source) { // Компиляция | Compile
    Lexer* lexer = lexer_create(source); // Создаём лексер | Create lexer

    int token_count = 0; // Количество токенов (заполнит лексер) | Token count (filled by lexer)
    Token* tokens = lexer_tokenize(lexer, &token_count); // Токенизируем исходник | Tokenize source

    if (lexer->had_error) { // Если лексер нашёл ошибку | If lexer found an error
        tokens_free(tokens, token_count); // Освобождаем токены | Free tokens
        lexer_destroy(lexer); // Освобождаем лексер | Free lexer
        return NULL; // Возвращаем NULL | Return NULL
    }

    Parser* parser = parser_create(tokens, token_count); // Создаём парсер | Create parser
    BlockNode* ast = parser_parse(parser); // Разбираем в AST | Parse into AST

    Bytecode* bc = ast ? codegen_generate(ast) : NULL; // Генерируем байт-код, если AST есть | Generate bytecode if AST exists

    /* AST хранит собственные копии строк, поэтому токены и лексер
       можно освобождать сразу после разбора. */
    // AST stores its own copies of strings, so tokens and lexer
    // can be freed right after parsing.
    ast_free((ASTNode*)ast); // Освобождаем AST | Free AST
    parser_destroy(parser); // Освобождаем парсер | Free parser
    tokens_free(tokens, token_count); // Освобождаем токены | Free tokens
    lexer_destroy(lexer); // Освобождаем лексер | Free lexer

    return bc; // Возвращаем байт-код | Return bytecode
}