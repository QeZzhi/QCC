// ============================================================
// parser.c — синтаксический анализатор (парсер) для QCC
// Превращает список токенов в AST (дерево разбора).
// parser.c — parser for QCC
// Converts a list of tokens into an AST (parse tree).
// ============================================================

#include "parser/parser.h" // Подключение заголовка парсера | Connecting parser header

#include <stdio.h>  // fprintf, snprintf | fprintf, snprintf
#include <stdlib.h> // atoi | atoi

#include "common/xalloc.h" // xmalloc | xmalloc

// ------------------------------------------------------------
// parser_create — создать парсер для массива токенов
// ------------------------------------------------------------
Parser* parser_create(Token* tokens, int count) { // Создание парсера | Create parser
    Parser* p = xmalloc(sizeof(Parser)); // Выделяем память под Parser | Allocate memory for Parser
    p->toks = tokens; // Массив токенов | Token array
    p->count = count; // Количество токенов | Token count
    p->pos = 0; // Текущая позиция — 0 | Current position — 0
    p->had_error = 0; // Ошибок пока нет | No errors yet
    return p; // Возвращаем парсер | Return parser
}

// ------------------------------------------------------------
// parser_destroy — освободить память парсера
// ------------------------------------------------------------
void parser_destroy(Parser* p) { free(p); } // Освобождаем парсер | Free parser

/* Зажим на случай pos == count: последний токен всегда TOK_END, поэтому
   чтения за границей массива не происходит. */
// Clamp for pos == count: last token is always TOK_END, so
// reading beyond the array never happens.
// ------------------------------------------------------------
// peek — посмотреть текущий токен без сдвига
// ------------------------------------------------------------
static Token peek(const Parser* p) { // Посмотреть токен | Peek token
    if (p->pos >= p->count) return p->toks[p->count - 1]; // Если конец — вернуть TOK_END | If end — return TOK_END
    return p->toks[p->pos]; // Иначе — текущий токен | Else — current token
}

// ------------------------------------------------------------
// previous — вернуть предыдущий токен
// ------------------------------------------------------------
static Token previous(const Parser* p) { return p->toks[p->pos - 1]; } // Предыдущий токен | Previous token

// ------------------------------------------------------------
// advance — сдвинуть позицию и вернуть текущий токен
// ------------------------------------------------------------
static Token advance(Parser* p) { // Сдвинуть позицию | Advance position
    Token token = peek(p); // Запоминаем текущий | Remember current
    if (p->pos < p->count) p->pos++; // Сдвигаем, если не конец | Advance if not end
    return token; // Возвращаем токен | Return token
}

// ------------------------------------------------------------
// check — проверить, что текущий токен имеет нужный тип
// ------------------------------------------------------------
static int check(const Parser* p, TokenType type) { return peek(p).type == type; } // Тип совпадает? | Type matches?

// ------------------------------------------------------------
// match — если тип совпадает, поглотить токен и вернуть 1
// ------------------------------------------------------------
static int match(Parser* p, TokenType type) { // Совпадение и поглощение | Match and consume
    if (!check(p, type)) return 0; // Если не совпало — 0 | If no match — 0
    advance(p); // Поглощаем токен | Consume token
    return 1; // Успех | Success
}

/* Печатаем только первую ошибку: после неё дерево разбора недостоверно,
   и лавина вторичных сообщений только мешает. */
// Print only the first error: after it the parse tree is unreliable,
// and a flood of secondary messages only gets in the way.
// ------------------------------------------------------------
// parser_error — вывести сообщение об ошибке (только первое)
// ------------------------------------------------------------
static void parser_error(Parser* p, int line, const char* message) { // Ошибка парсера | Parser error
    if (!p->had_error) { // Если ошибок ещё не было | If no errors yet
        fprintf(stderr, "error:%d: %s\n", line, message); // Печатаем | Print
    }
    p->had_error = 1; // Помечаем ошибку | Mark error
}

// ------------------------------------------------------------
// parser_error_expected — вывести «ожидался X, а найден Y»
// ------------------------------------------------------------
static void parser_error_expected(Parser* p, TokenType expected) { // Ожидаемый токен | Expected token
    char message[128]; // Буфер для сообщения | Message buffer
    snprintf(message, sizeof(message), "expected %s but found %s",
             token_type_name(expected), token_type_name(peek(p).type)); // Формируем сообщение | Format message
    parser_error(p, peek(p).line, message); // Печатаем | Print
}

/* 1 — токен нужного типа был и он поглощён; 0 — ошибка уже напечатана. */
// 1 — token of expected type was found and consumed; 0 — error already printed.
// ------------------------------------------------------------
// expect — ожидать токен определённого типа
// ------------------------------------------------------------
static int expect(Parser* p, TokenType type, Token* out) { // Ожидание токена | Expect token
    if (!check(p, type)) { // Если тип не совпал | If type doesn't match
        parser_error_expected(p, type); // Ошибка | Error
        return 0; // Провал | Failure
    }
    Token token = advance(p); // Поглощаем токен | Consume token
    if (out) *out = token; // Сохраняем, если нужно | Save if needed
    return 1; // Успех | Success
}

// ------------------------------------------------------------
// Объявления функций (forward declarations)
// Нужны, потому что функции вызывают друг друга рекурсивно
// ------------------------------------------------------------
static ASTNode* parse_expression(Parser* p); // Разбор выражения | Parse expression
static ASTNode* parse_term(Parser* p); // Разбор слагаемого | Parse term
static ASTNode* parse_factor(Parser* p); // Разбор множителя | Parse factor
static ASTNode* parse_primary(Parser* p); // Разбор первичного | Parse primary

// ------------------------------------------------------------
// parse_binary_right — разобрать правую часть бинарной операции
// ------------------------------------------------------------
static ASTNode* parse_binary_right(Parser* p, ASTNode* left, ASTNode* (*operand)(Parser*)) { // Правая часть | Right part
    char* op = advance(p).value; /* ast_binary_create копирует строку сразу */ // ast_binary_create copies string immediately
    ASTNode* right = operand(p); // Разбираем правый операнд | Parse right operand
    if (!right) { // Если ошибка | If error
        ast_free(left); // Освобождаем левый | Free left
        return NULL; // Возвращаем NULL | Return NULL
    }
    return ast_binary_create(op, left, right); // Создаём узел операции | Create operation node
}

/* expression := term
   (сравнения и логика появятся здесь, когда для них будут опкоды) */
// expression := term
// (comparisons and logic will appear here when opcodes exist for them)
// ------------------------------------------------------------
// parse_expression — разобрать выражение
// ------------------------------------------------------------
static ASTNode* parse_expression(Parser* p) { return parse_term(p); } // Пока просто term | Just term for now

/* term := factor (('+' | '-') factor)* */
// ------------------------------------------------------------
// parse_term — разобрать слагаемое (сложение и вычитание)
// ------------------------------------------------------------
static ASTNode* parse_term(Parser* p) { // Разбор слагаемого | Parse term
    ASTNode* left = parse_factor(p); // Разбираем множитель | Parse factor
    if (!left) return NULL; // Если ошибка — NULL | If error — NULL

    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) { // Пока + или - | While + or -
        ASTNode* node = parse_binary_right(p, left, parse_factor); // Разбираем правую часть | Parse right part
        if (!node) return NULL; // Если ошибка — NULL | If error — NULL
        left = node; // Обновляем левый | Update left
    }
    return left; // Возвращаем результат | Return result
}

/* factor := primary (('*' | '/') primary)* */
// ------------------------------------------------------------
// parse_factor — разобрать множитель (умножение и деление)
// ------------------------------------------------------------
static ASTNode* parse_factor(Parser* p) { // Разбор множителя | Parse factor
    ASTNode* left = parse_primary(p); // Разбираем первичное | Parse primary
    if (!left) return NULL; // Если ошибка — NULL | If error — NULL

    while (check(p, TOK_STAR) || check(p, TOK_SLASH)) { // Пока * или / | While * or /
        ASTNode* node = parse_binary_right(p, left, parse_primary); // Разбираем правую часть | Parse right part
        if (!node) return NULL; // Если ошибка — NULL | If error — NULL
        left = node; // Обновляем левый | Update left
    }
    return left; // Возвращаем результат | Return result
}

// ------------------------------------------------------------
// parse_primary — разобрать первичное выражение
// Число, переменная или выражение в скобках
// ------------------------------------------------------------
static ASTNode* parse_primary(Parser* p) { // Разбор первичного | Parse primary
    if (match(p, TOK_NUMBER)) { // Если число | If number
        return ast_number_create(atoi(previous(p).value)); // Создаём узел-число | Create number node
    }

    if (match(p, TOK_IDENT)) { // Если идентификатор | If identifier
        return ast_var_create(previous(p).value); // Создаём узел-переменную | Create variable node
    }

    if (match(p, TOK_LPAREN)) { // Если левая скобка | If left paren
        ASTNode* expr = parse_expression(p); // Разбираем выражение | Parse expression
        if (!expr) return NULL; // Если ошибка — NULL | If error — NULL
        if (!expect(p, TOK_RPAREN, NULL)) { // Ожидаем правую скобку | Expect right paren
            ast_free(expr); // Если нет — освобождаем | If not — free
            return NULL; // Возвращаем NULL | Return NULL
        }
        return expr; // Возвращаем выражение | Return expression
    }

    char message[128]; // Буфер для сообщения | Message buffer
    snprintf(message, sizeof(message), "expected expression but found %s",
             token_type_name(peek(p).type)); // Формируем сообщение | Format message
    parser_error(p, peek(p).line, message); // Ошибка | Error
    return NULL; // Возвращаем NULL | Return NULL
}

/* statement := 'let' IDENT '=' expression ';'
              | 'print' expression ';'
              | expression ';' */
// ------------------------------------------------------------
// parse_statement — разобрать один оператор
// ------------------------------------------------------------
static ASTNode* parse_statement(Parser* p) { // Разбор оператора | Parse statement
    if (match(p, TOK_LET)) { // Если let | If let
        Token name; // Имя переменной | Variable name
        if (!expect(p, TOK_IDENT, &name)) return NULL; // Ожидаем имя | Expect name
        if (!expect(p, TOK_ASSIGN, NULL)) return NULL; // Ожидаем = | Expect =

        ASTNode* value = parse_expression(p); // Разбираем выражение | Parse expression
        if (!value) return NULL; // Если ошибка — NULL | If error — NULL
        if (!expect(p, TOK_SEMICOLON, NULL)) { // Ожидаем ; | Expect ;
            ast_free(value); // Если нет — освобождаем | If not — free
            return NULL; // Возвращаем NULL | Return NULL
        }
        return ast_let_create(name.value, value); // Создаём узел let | Create let node
    }

    if (match(p, TOK_PRINT)) { // Если print | If print
        ASTNode* value = parse_expression(p); // Разбираем выражение | Parse expression
        if (!value) return NULL; // Если ошибка — NULL | If error — NULL
        if (!expect(p, TOK_SEMICOLON, NULL)) { // Ожидаем ; | Expect ;
            ast_free(value); // Если нет — освобождаем | If not — free
            return NULL; // Возвращаем NULL | Return NULL
        }
        return ast_print_create(value); // Создаём узел print | Create print node
    }

    ASTNode* expr = parse_expression(p); // Разбираем выражение | Parse expression
    if (!expr) return NULL; // Если ошибка — NULL | If error — NULL
    if (!expect(p, TOK_SEMICOLON, NULL)) { // Ожидаем ; | Expect ;
        ast_free(expr); // Если нет — освобождаем | If not — free
        return NULL; // Возвращаем NULL | Return NULL
    }
    return expr; // Возвращаем выражение | Return expression
}

// ------------------------------------------------------------
// parser_parse — разобрать весь исходник в блок AST
// ------------------------------------------------------------
BlockNode* parser_parse(Parser* p) { // Разбор всего | Parse everything
    BlockNode* block = ast_block_create(); // Создаём пустой блок | Create empty block

    while (!check(p, TOK_END)) { // Пока не конец | While not end
        ASTNode* stmt = parse_statement(p); // Разбираем оператор | Parse statement
        if (!stmt) { // Если ошибка | If error
            ast_free((ASTNode*)block); // Освобождаем блок | Free block
            return NULL; // Возвращаем NULL | Return NULL
        }
        ast_block_add(block, stmt); // Добавляем в блок | Add to block
    }

    return block; // Возвращаем блок | Return block
}