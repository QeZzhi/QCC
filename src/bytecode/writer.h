#ifndef QCC_BYTECODE_WRITER_H
#define QCC_BYTECODE_WRITER_H

#include "bytecode/bytecode.h"
#include <stdio.h>

// Сохранить байткод в бинарный файл .qc
// Возвращает 1 при успехе, 0 при ошибке
int bytecode_write_file(const Bytecode* bc, const char* filename);

#endif