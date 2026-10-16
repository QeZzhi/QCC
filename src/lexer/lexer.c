// ============================================================
// lexer.c — лексический анализатор (лексер) для QCC
// Превращает исходный код Qora в список токенов.
// lexer.c — lexical analyzer (lexer) for QCC
// Converts Qora source code into a list of tokens.
// ============================================================

#include "lexer/lexer.h" // Подключение заголовка лексера | Connecting lexer header

#include <ctype.h>  // isdigit, isalpha, isalnum, isprint | isdigit, isalpha, isalnum, isprint
#include <stdio.h>  // fprintf для ошибок | fprintf for errors
#include <string.h> // strlen, strcmp, memcpy | strlen, strcmp, memcpy

#include "common/xalloc.h" // xmalloc, xrealloc, xstrdup | xmalloc, xrealloc, xstrdup

#define LEXER_INITIAL_CAPACITY 16 // Начальная ёмкость массива токенов | Initial token array capacity

// ------------------------------------------------------------
// lexer_create — создать лексер для исходного кода
// ------------------------------------------------------------
Lexer* lexer_create(const char* source) { // Создание лексера | Create lexer
    Lexer* lexer = xmalloc(sizeof(Lexer)); // Выделяем память под Lexer | Allocate memory for Lexer
    lexer->src = xstrdup(source); // Копируем исходный код | Copy source code
    lexer->len = strlen(lexer->src); // Длина исходника | Source length
    lexer->pos = 0; // Текущая позиция — 0 | Current position — 0
    lexer->line = 1; // Текущая строка — 1 | Current line — 1
    lexer->had_error = 0; // Ошибок пока нет | No errors yet
    return lexer; // Возвращаем лексер | Return lexer
}

// ------------------------------------------------------------
// lexer_destroy — освободить память лексера
// ------------------------------------------------------------
void lexer_destroy(Lexer* lexer) { // Уничтожение лексера | Destroy lexer
    if (!lexer) return; // Защита от NULL | NULL protection
    free(lexer->src); // Освобождаем исходный код | Free source code
    free(lexer); // Освобождаем сам лексер | Free lexer itself
}

/* ctype-функции требуют unsigned char: на знаковом char кириллица
   в исходнике даёт неопределённое поведение. */
// ctype functions require unsigned char: on signed char, Cyrillic
// in source code leads to undefined behavior.
// ------------------------------------------------------------
// Вспомогательные функции для проверки символов
// ------------------------------------------------------------
static int is_digit(char c) { return isdigit((unsigned char)c) != 0; } // Это цифра? | Is it a digit?
static int is_ident_start(char c) { return isalpha((unsigned char)c) != 0 || c == '_'; } // Начало имени? | Start of identifier?
static int is_ident_char(char c) { return isalnum((unsigned char)c) != 0 || c == '_'; } // Символ имени? | Identifier character?

// ------------------------------------------------------------
// peek — посмотреть текущий символ без его сдвига
// ------------------------------------------------------------
static char peek(const Lexer* l) { // Посмотреть текущий символ | Peek current character
    return l->pos < l->len ? l->src[l->pos] : '\0'; // Если не конец — вернуть, иначе '\0' | If not end — return, else '\0'
}

// ------------------------------------------------------------
// advance — сдвинуть позицию на 1 символ и вернуть его
// ------------------------------------------------------------
static char advance(Lexer* l) { // Сдвинуть позицию | Advance position
    if (l->pos >= l->len) return '\0'; // Если конец — вернуть '\0' | If end — return '\0'
    char c = l->src[l->pos++]; // Берём символ и сдвигаем позицию | Take character and advance
    if (c == '\n') l->line++; // Если перевод строки — увеличить счётчик строк | If newline — increment line counter
    return c; // Возвращаем символ | Return character
}

// ------------------------------------------------------------
// is_at_end — проверить, закончился ли исходник
// ------------------------------------------------------------
static int is_at_end(const Lexer* l) { return l->pos >= l->len; } // Конец исходника? | End of source?

// ------------------------------------------------------------
// skip_whitespace — пропустить пробелы, табы и переводы строк
// ------------------------------------------------------------
static void skip_whitespace(Lexer* l) { // Пропуск пробелов | Skip whitespace
    while (!is_at_end(l)) { // Пока не конец | While not end
        char c = peek(l); // Текущий символ | Current character
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break; // Если не пробел — стоп | If not whitespace — stop
        advance(l); // Сдвигаем позицию | Advance position
    }
}

/* Неизвестный символ — это ошибка, а не повод молча его проглотить. */
// Unknown character is an error, not a reason to silently swallow it.
// ------------------------------------------------------------
// lexer_error_unexpected — вывести ошибку о неизвестном символе
// ------------------------------------------------------------
static void lexer_error_unexpected(Lexer* l, char c) { // Ошибка неизвестного символа | Unexpected character error
    fprintf(stderr, "error:%d: unexpected character '%c' (0x%02X)\n",
            l->line, // Номер строки | Line number
            isprint((unsigned char)c) ? c : '?', // Печатаемый символ или '?' | Printable char or '?'
            (unsigned char)c); // Шестнадцатеричный код | Hex code
    l->had_error = 1; // Помечаем ошибку | Mark error
}

/* Копия подстроки [start, start + len); владение у вызывающего. */
// Copy of substring [start, start + len); ownership belongs to caller.
// ------------------------------------------------------------
// substr_dup — скопировать подстроку исходника
// ------------------------------------------------------------
static char* substr_dup(const Lexer* l, size_t start, size_t len) { // Копия подстроки | Substring copy
    char* text = xmalloc(len + 1); // Выделяем память под подстроку | Allocate memory for substring
    memcpy(text, l->src + start, len); // Копируем | Copy
    text[len] = '\0'; // Завершаем строку | Terminate string
    return text; // Возвращаем копию | Return copy
}

// ------------------------------------------------------------
// make_token — создать токен (копирует значение)
// ------------------------------------------------------------
static Token make_token(TokenType type, const char* value, int line) { // Создание токена | Create token
    Token token; // Структура токена | Token struct
    token.type = type; // Тип токена | Token type
    token.value = xstrdup(value); // Копируем значение | Copy value
    token.line = line; // Номер строки | Line number
    return token; // Возвращаем токен | Return token
}

/* Забирает владение value — без лишней копии. */
// Takes ownership of value — without extra copy.
// ------------------------------------------------------------
// make_token_owned — создать токен, забрав владение строкой
// ------------------------------------------------------------
static Token make_token_owned(TokenType type, char* value, int line) { // Создание токена с владением | Create token with ownership
    Token token; // Структура токена | Token struct
    token.type = type; // Тип токена | Token type
    token.value = value; // Забираем указатель (не копируем) | Take pointer (no copy)
    token.line = line; // Номер строки | Line number
    return token; // Возвращаем токен | Return token
}

/* Строка берётся по указателю на исходник, поэтому переполнения буфера
   фиксированного размера здесь больше нет: длина числа не ограничена. */
// String is taken by pointer to source, so there's no fixed-size
// buffer overflow here anymore: number length is unlimited.
// ------------------------------------------------------------
// read_number — прочитать число из исходника
// ------------------------------------------------------------
static Token read_number(Lexer* l) { // Чтение числа | Read number
    int line = l->line; // Запоминаем строку | Remember line
    size_t start = l->pos; // Запоминаем начало | Remember start
    while (!is_at_end(l) && is_digit(peek(l))) { // Пока цифры | While digits
        advance(l); // Сдвигаем позицию | Advance position
    }
    return make_token_owned(TOK_NUMBER, substr_dup(l, start, l->pos - start), line); // Возвращаем токен-число | Return number token
}

// ------------------------------------------------------------
// keyword_type — определить тип ключевого слова
// ------------------------------------------------------------
static TokenType keyword_type(const char* text) { // Тип ключевого слова | Keyword type
    if (strcmp(text, "let") == 0)   return TOK_LET; // let | let
    if (strcmp(text, "print") == 0) return TOK_PRINT; // print | print
    if (strcmp(text, "if") == 0)    return TOK_IF; // if | if
    if (strcmp(text, "else") == 0)  return TOK_ELSE; // else | else
    if (strcmp(text, "while") == 0) return TOK_WHILE; // while | while
    return TOK_IDENT; // Иначе — идентификатор | Otherwise — identifier
}

// ------------------------------------------------------------
// read_ident — прочитать идентификатор или ключевое слово
// ------------------------------------------------------------
static Token read_ident(Lexer* l) { // Чтение идентификатора | Read identifier
    int line = l->line; // Запоминаем строку | Remember line
    size_t start = l->pos; // Запоминаем начало | Remember start
    while (!is_at_end(l) && is_ident_char(peek(l))) { // Пока символы имени | While identifier chars
        advance(l); // Сдвигаем позицию | Advance position
    }
    char* text = substr_dup(l, start, l->pos - start); // Копируем текст | Copy text
    return make_token_owned(keyword_type(text), text, line); // Возвращаем токен | Return token
}

// ------------------------------------------------------------
// lexer_tokenize — разбить исходник на токены
// ------------------------------------------------------------
Token* lexer_tokenize(Lexer* l, int* out_count) { // Токенизация | Tokenize
    int capacity = LEXER_INITIAL_CAPACITY; // Начальная ёмкость | Initial capacity
    int count = 0; // Счётчик токенов | Token counter
    Token* tokens = xmalloc(sizeof(Token) * (size_t)capacity); // Массив токенов | Token array
    
    while (!is_at_end(l)) { // Пока не конец | While not end
        skip_whitespace(l); // Пропускаем пробелы | Skip whitespace
        if (is_at_end(l)) break; // Если конец — выходим | If end — break
        
        /* +1, чтобы TOK_END гарантированно влез без отдельной проверки. */
        // +1 so TOK_END fits without separate check.
        if (count + 1 >= capacity) { // Если места мало | If space is low
            capacity *= 2; // Удваиваем ёмкость | Double capacity
            tokens = xrealloc(tokens, sizeof(Token) * (size_t)capacity); // Расширяем массив | Resize array
        }
        
        char c = peek(l); // Текущий символ | Current character
        
        if (is_digit(c)) { // Если цифра | If digit
            tokens[count++] = read_number(l); // Читаем число | Read number
            continue; // Продолжаем | Continue
        }
        
        if (is_ident_start(c)) { // Если начало имени | If identifier start
            tokens[count++] = read_ident(l); // Читаем имя | Read identifier
            continue; // Продолжаем | Continue
        }
        
        /* Номер строки фиксируется до advance(): '\n' увеличивает счётчик. */
        // Line number is fixed before advance(): '\n' increments the counter.
        int line = l->line; // Запоминаем строку | Remember line
        advance(l); // Сдвигаем позицию | Advance position
        
        switch (c) { // Обработка символа | Handle character
            case '=': tokens[count++] = make_token(TOK_ASSIGN, "=", line); break; // Присваивание | Assignment
            case '+': tokens[count++] = make_token(TOK_PLUS, "+", line); break; // Плюс | Plus
            case '-': tokens[count++] = make_token(TOK_MINUS, "-", line); break; // Минус | Minus
            case '*': tokens[count++] = make_token(TOK_STAR, "*", line); break; // Умножение | Multiply
            case '/': tokens[count++] = make_token(TOK_SLASH, "/", line); break; // Деление | Divide
            case '(': tokens[count++] = make_token(TOK_LPAREN, "(", line); break; // Левая скобка | Left paren
            case ')': tokens[count++] = make_token(TOK_RPAREN, ")", line); break; // Правая скобка | Right paren
            case '{': tokens[count++] = make_token(TOK_LBRACE, "{", line); break; // Левая фигурная | Left brace
            case '}': tokens[count++] = make_token(TOK_RBRACE, "}", line); break; // Правая фигурная | Right brace
            case ';': tokens[count++] = make_token(TOK_SEMICOLON, ";", line); break; // Точка с запятой | Semicolon
            default:  lexer_error_unexpected(l, c); break; // Ошибка | Error
        }
    }
    
    tokens[count++] = make_token(TOK_END, "", l->line); // Токен конца | End token
    *out_count = count; // Записываем количество | Write count
    return tokens; // Возвращаем токены | Return tokens
}