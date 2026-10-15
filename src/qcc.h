#ifndef QCC_H
#define QCC_H

#include "bytecode/bytecode.h"

#define QCC_VERSION "0.2.0"

/*
 * Компилирует исходник Qora в байткод.
 * При ошибке в исходнике возвращает NULL, напечатав сообщение в stderr;
 * процесс при этом не завершается.
 * Результат принадлежит вызывающему, освобождать через bytecode_free().
 */
Bytecode* qcc_compile(const char* source);

#endif
