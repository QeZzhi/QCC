// ============================================================
// parser.h — заголовок синтаксического анализатора для QCC
// Парсер превращает список токенов в AST (дерево разбора).
// parser.h — parser header for QCC
// Parser converts a list of tokens into an AST (parse tree).
// ============================================================

#ifndef QCC_PARSER_H // Если QCC_PARSER_H ещё не определён | If QCC_PARSER_H is not defined yet
#define QCC_PARSER_H // Определяем QCC_PARSER_H | Define QCC_PARSER_H

#include "ast/ast.h"        // Нужны типы AST (BlockNode, ASTNode) | Need AST types (BlockNode, ASTNode)
#include "lexer/token.h"    // Нужны типы Token и TokenType | Need Token and TokenType types

// ------------------------------------------------------------
// Parser — структура парсера
// Хранит массив токенов, позицию и флаг ошибки
// ------------------------------------------------------------
typedef struct { // Структура парсера | Parser struct
    Token* toks; // Массив токенов от лексера | Token array from lexer
    int count; // Количество токенов | Token count
    int pos; // Текущая позиция в массиве | Current position in array
    int had_error; // 1, если парсер уже сообщил об ошибке | 1 if parser already reported an error
} Parser; // Конец структуры Parser | End of Parser struct

// ------------------------------------------------------------
// Функции для работы с парсером
// ------------------------------------------------------------

// Создать парсер для массива токенов | Create parser for token array
Parser* parser_create(Token* tokens, int count);

// Освободить память парсера | Free parser memory
void parser_destroy(Parser* parser);

/* Возвращает NULL при синтаксической ошибке: сообщение уже напечатано
   в stderr, процесс не завершается. */
// Returns NULL on syntax error: message already printed
// to stderr, process does not exit.
// Разобрать весь исходник в блок AST | Parse entire source into AST block
BlockNode* parser_parse(Parser* parser);

#endif // Конец include guard | End of include guard