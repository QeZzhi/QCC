#include "lexer/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "common/xalloc.h"

#define LEXER_INITIAL_CAPACITY 16

Lexer* lexer_create(const char* source) {
    Lexer* lexer = xmalloc(sizeof(Lexer));
    lexer->src = xstrdup(source);
    lexer->len = strlen(lexer->src);
    lexer->pos = 0;
    lexer->line = 1;
    lexer->had_error = 0;
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (!lexer) return;
    free(lexer->src);
    free(lexer);
}

/* ctype-функции требуют unsigned char: на знаковом char кириллица
   в исходнике даёт неопределённое поведение. */
static int is_digit(char c) { return isdigit((unsigned char)c) != 0; }
static int is_ident_start(char c) { return isalpha((unsigned char)c) != 0 || c == '_'; }
static int is_ident_char(char c) { return isalnum((unsigned char)c) != 0 || c == '_'; }

static char peek(const Lexer* l) {
    return l->pos < l->len ? l->src[l->pos] : '\0';
}

static char advance(Lexer* l) {
    if (l->pos >= l->len) return '\0';
    char c = l->src[l->pos++];
    if (c == '\n') l->line++;
    return c;
}

static int is_at_end(const Lexer* l) { return l->pos >= l->len; }

static void skip_whitespace(Lexer* l) {
    while (!is_at_end(l)) {
        char c = peek(l);
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
        advance(l);
    }
}

/* Неизвестный символ — это ошибка, а не повод молча его проглотить. */
static void lexer_error_unexpected(Lexer* l, char c) {
    fprintf(stderr, "error:%d: unexpected character '%c' (0x%02X)\n",
            l->line,
            isprint((unsigned char)c) ? c : '?',
            (unsigned char)c);
    l->had_error = 1;
}

/* Копия подстроки [start, start + len); владение у вызывающего. */
static char* substr_dup(const Lexer* l, size_t start, size_t len) {
    char* text = xmalloc(len + 1);
    memcpy(text, l->src + start, len);
    text[len] = '\0';
    return text;
}

static Token make_token(TokenType type, const char* value, int line) {
    Token token;
    token.type = type;
    token.value = xstrdup(value);
    token.line = line;
    return token;
}

/* Забирает владение value — без лишней копии. */
static Token make_token_owned(TokenType type, char* value, int line) {
    Token token;
    token.type = type;
    token.value = value;
    token.line = line;
    return token;
}

/* Строка берётся по указателю на исходник, поэтому переполнения буфера
   фиксированного размера здесь больше нет: длина числа не ограничена. */
static Token read_number(Lexer* l) {
    int line = l->line;
    size_t start = l->pos;
    while (!is_at_end(l) && is_digit(peek(l))) {
        advance(l);
    }
    return make_token_owned(TOK_NUMBER, substr_dup(l, start, l->pos - start), line);
}

static TokenType keyword_type(const char* text) {
    if (strcmp(text, "let") == 0)   return TOK_LET;
    if (strcmp(text, "print") == 0) return TOK_PRINT;
    if (strcmp(text, "if") == 0)    return TOK_IF;
    if (strcmp(text, "else") == 0)  return TOK_ELSE;
    if (strcmp(text, "while") == 0) return TOK_WHILE;
    return TOK_IDENT;
}

static Token read_ident(Lexer* l) {
    int line = l->line;
    size_t start = l->pos;
    while (!is_at_end(l) && is_ident_char(peek(l))) {
        advance(l);
    }
    char* text = substr_dup(l, start, l->pos - start);
    return make_token_owned(keyword_type(text), text, line);
}

Token* lexer_tokenize(Lexer* l, int* out_count) {
    int capacity = LEXER_INITIAL_CAPACITY;
    int count = 0;
    Token* tokens = xmalloc(sizeof(Token) * (size_t)capacity);

    while (!is_at_end(l)) {
        skip_whitespace(l);
        if (is_at_end(l)) break;

        /* +1, чтобы TOK_END гарантированно влез без отдельной проверки. */
        if (count + 1 >= capacity) {
            capacity *= 2;
            tokens = xrealloc(tokens, sizeof(Token) * (size_t)capacity);
        }

        char c = peek(l);

        if (is_digit(c)) {
            tokens[count++] = read_number(l);
            continue;
        }

        if (is_ident_start(c)) {
            tokens[count++] = read_ident(l);
            continue;
        }

        /* Номер строки фиксируется до advance(): '\n' увеличивает счётчик. */
        int line = l->line;
        advance(l);

        switch (c) {
            case '=': tokens[count++] = make_token(TOK_ASSIGN, "=", line); break;
            case '+': tokens[count++] = make_token(TOK_PLUS, "+", line); break;
            case '-': tokens[count++] = make_token(TOK_MINUS, "-", line); break;
            case '*': tokens[count++] = make_token(TOK_STAR, "*", line); break;
            case '/': tokens[count++] = make_token(TOK_SLASH, "/", line); break;
            case '(': tokens[count++] = make_token(TOK_LPAREN, "(", line); break;
            case ')': tokens[count++] = make_token(TOK_RPAREN, ")", line); break;
            case '{': tokens[count++] = make_token(TOK_LBRACE, "{", line); break;
            case '}': tokens[count++] = make_token(TOK_RBRACE, "}", line); break;
            case ';': tokens[count++] = make_token(TOK_SEMICOLON, ";", line); break;
            default:  lexer_error_unexpected(l, c); break; /* токен не добавляем */
        }
    }

    tokens[count++] = make_token(TOK_END, "", l->line);
    *out_count = count;
    return tokens;
}
