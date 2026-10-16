// ============================================================
// codegen.h — заголовок генератора байт-кода для QCC
// Здесь описаны функции для превращения AST в байт-код.
// codegen.h — codegen header for QCC
// This file describes functions for converting AST into bytecode.
// ============================================================

#ifndef QCC_CODEGEN_H // Если QCC_CODEGEN_H ещё не определён | If QCC_CODEGEN_H is not defined yet
#define QCC_CODEGEN_H // Определяем QCC_CODEGEN_H | Define QCC_CODEGEN_H

#include "ast/ast.h"           // Нужны типы AST (BlockNode, ASTNode) | Need AST types (BlockNode, ASTNode)
#include "bytecode/bytecode.h" // Нужен тип Bytecode | Need Bytecode type

/* Возвращает NULL, если генерация невозможна (сообщение уже в stderr). */
// Returns NULL if generation is impossible (message already in stderr).
// ------------------------------------------------------------
// codegen_generate — сгенерировать байт-код для всего блока
// Принимает корневой блок AST, возвращает байт-код (или NULL при ошибке)
// ------------------------------------------------------------
Bytecode* codegen_generate(BlockNode* block); // Генерация байт-кода для блока | Generate bytecode for block

#endif // Конец include guard | End of include guard