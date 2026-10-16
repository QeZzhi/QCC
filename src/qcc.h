// ============================================================
// qcc.h — главный заголовок компилятора QCC
// Описывает публичный API компилятора.
// qcc.h — main header of QCC compiler
// Describes the public API of the compiler.
// ============================================================

#ifndef QCC_H // Если QCC_H ещё не определён | If QCC_H is not defined yet
#define QCC_H // Определяем QCC_H | Define QCC_H

#include "bytecode/bytecode.h" // Нужен тип Bytecode | Need Bytecode type

#define QCC_VERSION "0.2.0" // Версия компилятора | Compiler version

/*
 * Компилирует исходник Qora в байткод.
 * При ошибке в исходнике возвращает NULL, напечатав сообщение в stderr;
 * процесс при этом не завершается.
 * Результат принадлежит вызывающему, освобождать через bytecode_free().
 */
// Compiles Qora source into bytecode.
// On error in source, returns NULL after printing a message to stderr;
// the process does not exit.
// The result belongs to the caller, free it via bytecode_free().
// ------------------------------------------------------------
// qcc_compile — скомпилировать исходник Qora в байт-код
// ------------------------------------------------------------
Bytecode* qcc_compile(const char* source); // Компиляция исходника | Compile source

#endif // Конец include guard | End of include guard