// ============================================================
// bytecode.h — заголовок байт-кода для QCC
// Здесь описаны типы и функции для работы с байт-кодом.
// bytecode.h — bytecode header for QCC
// This file describes types and functions for working with bytecode.
// ============================================================

#ifndef QCC_BYTECODE_H // Если QCC_BYTECODE_H ещё не определён | If QCC_BYTECODE_H is not defined yet
#define QCC_BYTECODE_H // Определяем QCC_BYTECODE_H | Define QCC_BYTECODE_H

// ------------------------------------------------------------
// OpCode — коды операций байт-кода
// Каждая инструкция имеет свой код операции
// ------------------------------------------------------------
typedef enum { // Перечисление кодов операций | Enum of operation codes
    OP_PUSH,   // Положить значение на стек | Push value to stack
    OP_LOAD,   // Загрузить переменную на стек | Load variable to stack
    OP_STORE,  // Сохранить значение в переменную | Store value to variable
    OP_ADD,    // Сложение | Addition
    OP_SUB,    // Вычитание | Subtraction
    OP_MUL,    // Умножение | Multiplication
    OP_DIV,    // Деление | Division
    OP_PRINT,  // Вывод на экран | Print to screen
    OP_HALT    // Остановка выполнения | Halt execution
} OpCode; // Конец перечисления OpCode | End of OpCode enum

// ------------------------------------------------------------
// Instruction — одна инструкция байт-кода
// op — что делать, arg — с чем (число, имя переменной)
// ------------------------------------------------------------
typedef struct { // Структура одной инструкции | Struct of a single instruction
    OpCode op; // Код операции | Operation code
    char* arg; // Аргумент (число или имя переменной) | Argument (number or variable name)
} Instruction; // Конец структуры Instruction | End of Instruction struct

// ------------------------------------------------------------
// Bytecode — весь байт-код программы
// Это массив инструкций + счётчик + ёмкость
// ------------------------------------------------------------
typedef struct { // Структура байт-кода | Bytecode struct
    Instruction* instructions; // Массив инструкций | Array of instructions
    int count; // Сколько инструкций сейчас | Number of instructions now
    int capacity; // Сколько инструкций влезает | How many instructions fit
} Bytecode; // Конец структуры Bytecode | End of Bytecode struct

// ------------------------------------------------------------
// Функции для работы с байт-кодом
// ------------------------------------------------------------

// Создать пустой байт-код | Create empty bytecode
Bytecode* bytecode_create(void);

// Добавить инструкцию в байт-код | Add instruction to bytecode
void bytecode_add(Bytecode* bc, OpCode op, const char* arg);

// Освободить память байт-кода | Free bytecode memory
void bytecode_free(Bytecode* bc);

/* Печатает дизассемблированный байткод в stdout.
   Пока это единственный способ увидеть результат компиляции — VM ещё нет. */
// Печатает байт-код в stdout для отладки | Prints bytecode to stdout for debugging
void bytecode_dump(const Bytecode* bc);

#endif // Конец include guard | End of include guard