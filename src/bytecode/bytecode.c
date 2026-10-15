#include "bytecode/bytecode.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/xalloc.h"

#define BYTECODE_INITIAL_CAPACITY 16

Bytecode* bytecode_create(void) {
    Bytecode* bc = xmalloc(sizeof(Bytecode));
    bc->capacity = BYTECODE_INITIAL_CAPACITY;
    bc->count = 0;
    bc->instructions = xmalloc(sizeof(Instruction) * (size_t)bc->capacity);
    return bc;
}

void bytecode_add(Bytecode* bc, OpCode op, const char* arg) {
    if (bc->count >= bc->capacity) {
        bc->capacity *= 2;
        bc->instructions = xrealloc(bc->instructions,
                                    sizeof(Instruction) * (size_t)bc->capacity);
    }
    bc->instructions[bc->count].op = op;
    bc->instructions[bc->count].arg = xstrdup(arg);
    bc->count++;
}

void bytecode_free(Bytecode* bc) {
    if (!bc) return;
    for (int i = 0; i < bc->count; i++) {
        free(bc->instructions[i].arg);
    }
    free(bc->instructions);
    free(bc);
}

static const char* opcode_name(OpCode op) {
    switch (op) {
        case OP_PUSH:  return "PUSH";
        case OP_LOAD:  return "LOAD";
        case OP_STORE: return "STORE";
        case OP_ADD:   return "ADD";
        case OP_SUB:   return "SUB";
        case OP_MUL:   return "MUL";
        case OP_DIV:   return "DIV";
        case OP_PRINT: return "PRINT";
        case OP_HALT:  return "HALT";
    }
    return "???";
}

void bytecode_dump(const Bytecode* bc) {
    for (int i = 0; i < bc->count; i++) {
        const Instruction* ins = &bc->instructions[i];
        printf("%4d  %-5s", i, opcode_name(ins->op));
        if (ins->arg && ins->arg[0] != '\0') {
            printf(" %s", ins->arg);
        }
        printf("\n");
    }
}
