#include "bytecode/writer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define QEZVM_MAGIC 0x515A564D // "QZVM"

int bytecode_write_file(const Bytecode* bc, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) return 0;

    // 1. Записываем магию
    uint32_t magic = QEZVM_MAGIC;
    fwrite(&magic, sizeof(uint32_t), 1, file);

    // 2. Записываем количество инструкций
    uint32_t count = bc->count;
    fwrite(&count, sizeof(uint32_t), 1, file);

    // 3. Записываем инструкции
    for (int i = 0; i < bc->count; i++) {
        const Instruction* ins = &bc->instructions[i];
        
        // Записываем ТОЛЬКО опкод (1 байт)
        uint8_t op = (uint8_t)ins->op;
        fwrite(&op, sizeof(uint8_t), 1, file);

        // Записываем аргумент ТОЛЬКО если он есть в опкоде
        if (ins->op == OP_PUSH) {
            int32_t val = atoi(ins->arg ? ins->arg : "0");
            fwrite(&val, sizeof(int32_t), 1, file);
        } 
        // === ОБНОВЛЕНО: Добавлен OP_PRINT_STR ===
        else if (ins->op == OP_LOAD || ins->op == OP_STORE || ins->op == OP_PRINT_STR) {
            uint32_t len = ins->arg ? (uint32_t)strlen(ins->arg) : 0;
            fwrite(&len, sizeof(uint32_t), 1, file);
            if (len > 0) {
                fwrite(ins->arg, 1, len, file);
            }
        }
        // Для OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_PRINT, OP_HALT 
        // МЫ НИЧЕГО НЕ ПИШЕМ. Опкод закончился, идем к следующему.
    }

    fclose(file);
    return 1;
}