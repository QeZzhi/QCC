// ============================================================
// bytecode.c — реализация байт-кода для QCC
// Байт-код — это список инструкций для QezVM (PUSH, ADD, PRINT и т.д.)
// bytecode.c — implementation of bytecode for QCC
// Bytecode is a list of instructions for QezVM (PUSH, ADD, PRINT, etc.)
// ============================================================


#include "bytecode/bytecode.h" // Подключение заголовка байт-кода | Connecting bytecode header
#include <stdio.h>  // printf для отладки | printf for debugging
#include <stdlib.h> // free для освобождения памяти | free for memory release
#include "common/xalloc.h" // xmalloc, xrealloc, xstrdup | xmalloc, xrealloc, xstrdup

#define BYTECODE_INITIAL_CAPACITY 16 // Начальная ёмкость массива инструкций | Initial instruction array capacity

// ------------------------------------------------------------
// bytecode_create — создать пустой байт-код
// ------------------------------------------------------------
Bytecode* bytecode_create(void) { // Функция создания байт-кода | Function to create bytecode
    Bytecode* bc = xmalloc(sizeof(Bytecode)); // Выделяем память под структуру Bytecode | Allocate memory for Bytecode struct
    bc->capacity = BYTECODE_INITIAL_CAPACITY; // Устанавливаем начальную ёмкость — 16 | Set initial capacity — 16
    bc->count = 0; // Инструкций пока нет | No instructions yet
    bc->instructions = xmalloc(sizeof(Instruction) * (size_t)bc->capacity); // Выделяем память под массив инструкций | Allocate memory for instruction array
    return bc; // Возвращаем указатель на байт-код | Return pointer to bytecode
} // Скобка закрывающая тело функции bytecode_create | Bracket covering the body of the function bytecode_create

// ------------------------------------------------------------
// bytecode_add — добавить инструкцию в байт-код
// op — код операции (PUSH, ADD и т.д.)
// arg — аргумент (число, имя переменной) или NULL
// ------------------------------------------------------------
void bytecode_add(Bytecode* bc, OpCode op, const char* arg) { // Функция добавления инструкции | Function to add instruction
    if (bc->count >= bc->capacity) { // Если места в массиве нет | If no space in array
        bc->capacity *= 2; // Увеличиваем ёмкость в 2 раза | Double the capacity
        bc->instructions = xrealloc(bc->instructions,
                                    sizeof(Instruction) * (size_t)bc->capacity); // Расширяем массив | Resize array
    } // Скобка закрывающая тело if проверки | Bracket covering the body of the check if
    bc->instructions[bc->count].op = op; // Записываем код операции | Write operation code
    bc->instructions[bc->count].arg = xstrdup(arg); // Копируем аргумент | Copy argument
    bc->count++; // Увеличиваем счётчик инструкций | Increment instruction counter
} // Скобка закрывающая тело функции bytecode_add | Bracket covering the body of the function bytecode_add

// ------------------------------------------------------------
// bytecode_free — освободить память байт-кода
// ------------------------------------------------------------
void bytecode_free(Bytecode* bc) { // Функция освобождения памяти | Function to free memory
    if (!bc) return; // Защита от NULL | NULL protection
    for (int i = 0; i < bc->count; i++) { // Проходим по всем инструкциям | Loop over all instructions
        free(bc->instructions[i].arg); // Освобождаем аргумент каждой инструкции | Free argument of each instruction
    } // Скобка закрывающая тело цикла for | Bracket covering the body of the for loop
    free(bc->instructions); // Освобождаем массив инструкций | Free instruction array
    free(bc); // Освобождаем саму структуру Bytecode | Free Bytecode struct itself
} // Скобка закрывающая тело функции bytecode_free | Bracket covering the body of the function bytecode_free

// ------------------------------------------------------------
// opcode_name — вернуть имя операции по её коду
// Используется для отладки (bytecode_dump)
// ------------------------------------------------------------
static const char* opcode_name(OpCode op) { // Функция получения имени операции | Function to get operation name
    switch (op) { // Смотрим на код операции | Check operation code
        case OP_PUSH:  return "PUSH"; // Положить на стек | Push to stack
        case OP_LOAD:  return "LOAD"; // Загрузить переменную | Load variable
        case OP_STORE: return "STORE"; // Сохранить в переменную | Store to variable
        case OP_ADD:   return "ADD"; // Сложение | Addition
        case OP_SUB:   return "SUB"; // Вычитание | Subtraction
        case OP_MUL:   return "MUL"; // Умножение | Multiplication
        case OP_DIV:   return "DIV"; // Деление | Division
        case OP_PRINT: return "PRINT"; // Вывод | Print
        case OP_HALT:  return "HALT"; // Остановка | Halt
    } // Скобка закрывающая проверку switch | Bracket that closes the switch check |
    return "???"; // Неизвестная операция | Unknown operation
} // Скобка закрывающая тело массива opcode_name | Bracket covering the body of the opcode_name array

// ------------------------------------------------------------
// bytecode_dump — вывести все инструкции на экран
// Для отладки: смотрим, что сгенерировал QCC
// ------------------------------------------------------------
void bytecode_dump(const Bytecode* bc) { // Функция вывода байт-кода | Function to dump bytecode
    for (int i = 0; i < bc->count; i++) { // Проходим по всем инструкциям | Loop over all instructions
        const Instruction* ins = &bc->instructions[i]; // Берём текущую инструкцию | Get current instruction
        printf("%4d  %-5s", i, opcode_name(ins->op)); // Печатаем номер и имя операции | Print index and operation name
        if (ins->arg && ins->arg[0] != '\0') { // Если аргумент есть и не пустой | If argument exists and not empty
            printf(" %s", ins->arg); // Печатаем аргумент | Print argument
        } // Скобка закрывающая тело if проверки | Bracket covering the body of the check if
        printf("\n"); // Перенос строки | New line
    } // Скобка закрывающая тело цикла for | Bracket covering the body of the for loop
} // Скобка закрывающая тело функции bytecode_dump | Bracket covering the body of the function bytecode_dump