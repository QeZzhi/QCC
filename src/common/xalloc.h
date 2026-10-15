#ifndef QCC_XALLOC_H
#define QCC_XALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Аллокаторы, которые не возвращают NULL: при нехватке памяти компилятор
 * печатает сообщение и завершается. Благодаря этому в остальном коде нет
 * проверок на OOM в каждом вызове, а возврат NULL везде означает только
 * ошибку в исходнике Qora.
 */

static inline void* xmalloc(size_t size) {
    void* ptr = malloc(size ? size : 1);
    if (!ptr) {
        fprintf(stderr, "fatal: out of memory (%zu bytes)\n", size);
        exit(1);
    }
    return ptr;
}

static inline void* xrealloc(void* ptr, size_t size) {
    void* grown = realloc(ptr, size ? size : 1);
    if (!grown) {
        fprintf(stderr, "fatal: out of memory (%zu bytes)\n", size);
        exit(1);
    }
    return grown;
}

/* Для NULL возвращает NULL — удобно для необязательных полей. */
static inline char* xstrdup(const char* text) {
    if (!text) return NULL;
    size_t size = strlen(text) + 1;
    char* copy = xmalloc(size);
    memcpy(copy, text, size);
    return copy;
}

#endif
