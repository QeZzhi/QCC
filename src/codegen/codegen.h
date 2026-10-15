#ifndef QCC_CODEGEN_H
#define QCC_CODEGEN_H

#include "ast/ast.h"
#include "bytecode/bytecode.h"

/* Возвращает NULL, если генерация невозможна (сообщение уже в stderr). */
Bytecode* codegen_generate(BlockNode* block);

#endif
