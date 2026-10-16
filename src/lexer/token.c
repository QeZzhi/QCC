// ============================================================
// token.c — реализация токенов для QCC
// Токен — это минимальная единица кода (число, имя, оператор).
// token.c — token implementation for QCC
// Token is the smallest unit of code (number, identifier, operator).
// ============================================================

#include "lexer/token.h" // Подключение заголовка токенов | Connecting token header

#include <stdlib.h> // free | free

// ------------------------------------------------------------
// token_type_name — вернуть человекочитаемое имя типа токена
// Используется в сообщениях об ошибках парсера
// ------------------------------------------------------------
const char* token_type_name(TokenType type) { // Имя типа токена | Token type name
    switch (type) { // Смотрим на тип | Check type
        case TOK_LET:       return "'let'";          // Ключевое слово let | let keyword
        case TOK_IDENT:     return "identifier";     // Идентификатор | Identifier
        case TOK_NUMBER:    return "number";         // Число | Number
        case TOK_ASSIGN:    return "'='";            // Присваивание | Assignment
        case TOK_PLUS:      return "'+'";            // Плюс | Plus
        case TOK_MINUS:     return "'-'";            // Минус | Minus
        case TOK_STAR:      return "'*'";            // Умножение | Multiply
        case TOK_SLASH:     return "'/'";            // Деление | Divide
        case TOK_LPAREN:    return "'('";            // Левая скобка | Left paren
        case TOK_RPAREN:    return "')'";            // Правая скобка | Right paren
        case TOK_PRINT:     return "'print'";        // Ключевое слово print | print keyword
        case TOK_IF:        return "'if'";           // Ключевое слово if | if keyword
        case TOK_ELSE:      return "'else'";         // Ключевое слово else | else keyword
        case TOK_WHILE:     return "'while'";        // Ключевое слово while | while keyword
        case TOK_LBRACE:    return "'{'";            // Левая фигурная | Left brace
        case TOK_RBRACE:    return "'}'";            // Правая фигурная | Right brace
        case TOK_SEMICOLON: return "';'";            // Точка с запятой | Semicolon
        case TOK_END:       return "end of file";    // Конец файла | End of file
    }
    return "unknown token"; // Неизвестный токен | Unknown token
}

// ------------------------------------------------------------
// token_free — освободить память одного токена
// Освобождает только value (сам токен — на стеке или в массиве)
// ------------------------------------------------------------
void token_free(Token* token) { // Освобождение токена | Free token
    if (!token) return; // Защита от NULL | NULL protection
    free(token->value); // Освобождаем строку значения | Free value string
    token->value = NULL; // Обнуляем указатель (защита от двойного free) | Set to NULL (double-free protection)
}

// ------------------------------------------------------------
// tokens_free — освободить массив токенов
// Проходит по всем токенам и освобождает каждый
// ------------------------------------------------------------
void tokens_free(Token* tokens, int count) { // Освобождение массива токенов | Free token array
    if (!tokens) return; // Защита от NULL | NULL protection
    for (int i = 0; i < count; i++) { // Проходим по всем токенам | Loop over all tokens
        token_free(&tokens[i]); // Освобождаем каждый | Free each one
    }
    free(tokens); // Освобождаем сам массив | Free array itself
}