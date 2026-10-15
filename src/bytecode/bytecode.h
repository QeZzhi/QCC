#ifndef QCC_BYTECODE_H
#define QCC_BYTECODE_H

typedef enum {
    OP_PUSH,
    OP_LOAD,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT,
    OP_HALT
} OpCode;

typedef struct {
    OpCode op;
    char* arg;
} Instruction;

typedef struct {
    Instruction* instructions;
    int count;
    int capacity;
} Bytecode;

Bytecode* bytecode_create(void);
void bytecode_add(Bytecode* bc, OpCode op, const char* arg);
void bytecode_free(Bytecode* bc);

/* Печатает дизассемблированный байткод в stdout.
   Пока это единственный способ увидеть результат компиляции — VM ещё нет. */
void bytecode_dump(const Bytecode* bc);

#endif
