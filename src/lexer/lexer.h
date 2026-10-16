// ============================================================
// lexer.h — заголовок лексического анализатора для QCC
// Лексер превращает исходный код Qora в список токенов.
// lexer.h — lexer header for QCC
// Lexer converts Qora source code into a list of tokens.
// ============================================================

#ifndef QCC_LEXER_H // Если QCC_LEXER_H ещё не определён | If QCC_LEXER_H is not defined yet
#define QCC_LEXER_H // Определяем QCC_LEXER_H | Define QCC_LEXER_H

#include <stddef.h> // size_t | size_t

#include "lexer/token.h" // Типы Token и TokenType | Token and TokenType types

// ------------------------------------------------------------
// Lexer — структура лексера
// Хранит исходный код, текущую позицию, номер строки
// ------------------------------------------------------------
typedef struct { // Структура лексера | Lexer struct
    char* src;  /* владеемая копия исходника */ // Owned copy of source
    size_t len; /* длина src, считается один раз при создании */ // Length of src, computed once at creation
    size_t pos; // Текущая позиция в исходнике | Current position in source
    int line; // Текущий номер строки | Current line number
    int had_error; /* 1, если лексер уже сообщил об ошибке */ // 1 if lexer already reported an error
} Lexer; // Конец структуры Lexer | End of Lexer struct

// ------------------------------------------------------------
// Функции для работы с лексером
// ------------------------------------------------------------

// Создать лексер для исходного кода | Create lexer for source code
Lexer* lexer_create(const char* source);

// Освободить память лексера | Free lexer memory
void lexer_destroy(Lexer* lexer);

/* Возвращает массив токенов, всегда заканчивающийся TOK_END.
   Сообщения об ошибках печатаются в stderr, признак — lexer->had_error. */
// Returns an array of tokens, always ending with TOK_END.
// Error messages are printed to stderr, flag — lexer->had_error.
// Токенизировать исходник | Tokenize source
Token* lexer_tokenize(Lexer* lexer, int* out_count);

#endif // Конец include guard | End of include guard