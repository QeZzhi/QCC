// ============================================================
// token.h — заголовок токенов для QCC
// Токен — это минимальная единица кода (число, имя, оператор).
// token.h — token header for QCC
// Token is the smallest unit of code (number, identifier, operator).
// ============================================================

#ifndef QCC_TOKEN_H // Если QCC_TOKEN_H ещё не определён | If QCC_TOKEN_H is not defined yet
#define QCC_TOKEN_H // Определяем QCC_TOKEN_H | Define QCC_TOKEN_H

// ------------------------------------------------------------
// TokenType — типы токенов
// Каждый токен имеет свой тип (число, имя, оператор)
// ------------------------------------------------------------
typedef enum {
    TOK_LET, 
    TOK_IDENT, 
    TOK_NUMBER, 
    TOK_STRING, // <--- Добавили TOK_STRING
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
    TOK_COMMA, // <--- Добавили TOK_COMMA
    TOK_END
} TokenType;

// ------------------------------------------------------------
// Token — один токен
// type — что это, value — текст, line — строка в исходнике
// ------------------------------------------------------------
typedef struct { // Структура токена | Token struct
    TokenType type; // Тип токена | Token type
    char* value; /* владеемая копия лексемы, может быть NULL */ // Owned copy of lexeme, may be NULL
    int line; // Номер строки в исходнике | Line number in source
} Token; // Конец структуры Token | End of Token struct

/* Имя типа токена для сообщений об ошибках ("';'", "identifier", ...). */
// Token type name for error messages ("';'", "identifier", ...).
// Вернуть читаемое имя типа токена | Return human-readable token type name
const char* token_type_name(TokenType type);

// Освободить память одного токена | Free memory of a single token
void token_free(Token* token);

// Освободить память массива токенов | Free memory of token array
void tokens_free(Token* tokens, int count);

#endif // Конец include guard | End of include guard