// ============================================================
// xalloc.h — безопасные аллокаторы памяти для QCC
// Обёртки над malloc/realloc/strdup с проверкой ошибок.
// Если память не выделилась — программа завершается с сообщением.
// xalloc.h — safe memory allocators for QCC
// Wrappers over malloc/realloc/strdup with error checking.
// If memory allocation fails — program exits with message.
// ============================================================

#ifndef QCC_XALLOC_H // Если QCC_XALLOC_H ещё не определён | If QCC_XALLOC_H is not defined yet
#define QCC_XALLOC_H // Определяем QCC_XALLOC_H | Define QCC_XALLOC_H

#include <stdio.h>  // fprintf для сообщения об ошибке | fprintf for error message
#include <stdlib.h> // malloc, realloc, exit | malloc, realloc, exit
#include <string.h> // strlen, memcpy | strlen, memcpy

/*
 * Аллокаторы, которые не возвращают NULL: при нехватке памяти компилятор
 * печатает сообщение и завершается. Благодаря этому в остальном коде нет
 * проверок на OOM в каждом вызове, а возврат NULL везде означает только
 * ошибку в исходнике Qora.
 */
// Allocators that never return NULL: on out-of-memory the compiler
// prints a message and exits. Thanks to this, the rest of the code
// has no OOM checks in every call, and NULL return everywhere means
// only an error in the Qora source code.

// ------------------------------------------------------------
// xmalloc — выделить память (с проверкой ошибок)
// Если size == 0 — выделяем 1 байт (malloc(0) ведёт себя странно)
// ------------------------------------------------------------
static inline void* xmalloc(size_t size) { // Безопасный malloc | Safe malloc
    void* ptr = malloc(size ? size : 1); // Выделяем память (минимум 1 байт) | Allocate memory (at least 1 byte)
    if (!ptr) { // Если память не выделилась | If memory allocation failed
        fprintf(stderr, "fatal: out of memory (%zu bytes)\n", size); // Сообщение об ошибке | Error message
        exit(1); // Завершаем программу | Exit the program
    }
    return ptr; // Возвращаем указатель | Return pointer
}

// ------------------------------------------------------------
// xrealloc — перевыделить память (с проверкой ошибок)
// Если size == 0 — выделяем 1 байт
// ------------------------------------------------------------
static inline void* xrealloc(void* ptr, size_t size) { // Безопасный realloc | Safe realloc
    void* grown = realloc(ptr, size ? size : 1); // Перевыделяем память | Reallocate memory
    if (!grown) { // Если память не выделилась | If memory allocation failed
        fprintf(stderr, "fatal: out of memory (%zu bytes)\n", size); // Сообщение об ошибке | Error message
        exit(1); // Завершаем программу | Exit the program
    }
    return grown; // Возвращаем новый указатель | Return new pointer
}

/* Для NULL возвращает NULL — удобно для необязательных полей. */
// For NULL returns NULL — useful for optional fields.
// ------------------------------------------------------------
// xstrdup — скопировать строку (с проверкой ошибок)
// Если text == NULL — возвращает NULL (не копирует)
// ------------------------------------------------------------
static inline char* xstrdup(const char* text) { // Безопасный strdup | Safe strdup
    if (!text) return NULL; // Если строка NULL — возвращаем NULL | If string is NULL — return NULL
    size_t size = strlen(text) + 1; // Длина строки + 1 (для \0) | String length + 1 (for \0)
    char* copy = xmalloc(size); // Выделяем память под копию | Allocate memory for copy
    memcpy(copy, text, size); // Копируем строку | Copy string
    return copy; // Возвращаем копию | Return copy
}

#endif // Конец include guard | End of include guard