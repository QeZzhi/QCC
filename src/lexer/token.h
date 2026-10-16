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
typedef enum { // Перечисление типов токенов | Enum of token types
    TOK_LET,       // Ключевое слово let | let keyword
    TOK_IDENT,     // Идентификатор (имя переменной) | Identifier (variable name)
    TOK_NUMBER,    // Число | Number
    TOK_ASSIGN,    // Присваивание (=) | Assignment (=)
    TOK_PLUS,      // Плюс (+) | Plus (+)
    TOK_MINUS,     // Минус (-) | Minus (-)
    TOK_STAR,      // Умножение (*) | Multiply (*)
    TOK_SLASH,     // Деление (/) | Divide (/)
    TOK_LPAREN,    // Левая скобка (() | Left paren (()
    TOK_RPAREN,    // Правая скобка ()) | Right paren ())
    TOK_PRINT,     // Ключевое слово print | print keyword
    TOK_IF,        // Ключевое слово if | if keyword
    TOK_ELSE,      // Ключевое слово else | else keyword
    TOK_WHILE,     // Ключевое слово while | while keyword
    TOK_LBRACE,    // Левая фигурная ({) | Left brace ({)
    TOK_RBRACE,    // Правая фигурная (}) | Right brace (})
    TOK_SEMICOLON, // Точка с запятой (;) | Semicolon (;)
    TOK_END        // Конец файла | End of file
} TokenType; // Конец перечисления | End of enum

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