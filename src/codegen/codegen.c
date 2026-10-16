// ============================================================
// codegen.c — генератор байт-кода для QCC
// Превращает AST (дерево разбора) в байт-код для QezVM.
// codegen.c — bytecode generator for QCC
// Converts AST (parse tree) into bytecode for QezVM.
// ============================================================

#include "codegen/codegen.h" // Подключение заголовка codegen | Connecting codegen header

#include <stdio.h>  // fprintf, snprintf | fprintf, snprintf
#include <string.h> // strcmp | strcmp

// 0 — успех, -1 — ошибка (сообщение уже напечатано)
// 0 — success, -1 — error (message already printed)
// ------------------------------------------------------------
// gen_node — рекурсивно генерирует байт-код для одного узла AST
// ------------------------------------------------------------
static int gen_node(Bytecode* bc, const ASTNode* node) { // Генерация байт-кода для узла | Generate bytecode for node
    switch (node->type) { // Смотрим тип узла | Check node type
        
        // ----- Узел-число -----
        case NODE_NUMBER: { // Число | Number
            const NumberNode* n = (const NumberNode*)node; // Приводим к NumberNode | Cast to NumberNode
            char buf[32]; // Буфер для числа | Buffer for number
            snprintf(buf, sizeof(buf), "%d", n->value); // Превращаем число в строку | Convert number to string
            bytecode_add(bc, OP_PUSH, buf); // Добавляем инструкцию PUSH | Add PUSH instruction
            return 0; // Успех | Success
        }
        
        // ----- Узел-переменная -----
        case NODE_VAR: { // Переменная | Variable
            const VarNode* n = (const VarNode*)node; // Приводим к VarNode | Cast to VarNode
            bytecode_add(bc, OP_LOAD, n->name); // Добавляем инструкцию LOAD | Add LOAD instruction
            return 0; // Успех | Success
        }
        
        // ----- Узел-объявление переменной (let) -----
        case NODE_LET: { // Let | Let
            const LetNode* n = (const LetNode*)node; // Приводим к LetNode | Cast to LetNode
            if (gen_node(bc, n->value) != 0) return -1; // Сначала генерируем выражение справа | First generate right expression
            bytecode_add(bc, OP_STORE, n->name); // Потом сохраняем в переменную | Then store to variable
            return 0; // Успех | Success
        }
        
        // ----- Узел-вывод (print) -----
        case NODE_PRINT: { // Print | Print
            const PrintNode* n = (const PrintNode*)node; // Приводим к PrintNode | Cast to PrintNode
            if (gen_node(bc, n->value) != 0) return -1; // Генерируем выражение | Generate expression
            bytecode_add(bc, OP_PRINT, ""); // Добавляем инструкцию PRINT | Add PRINT instruction
            return 0; // Успех | Success
        }
        
        // ----- Узел-бинарная операция -----
        case NODE_BINARY_OP: { // Операция | Operation
            const BinaryOpNode* n = (const BinaryOpNode*)node; // Приводим к BinaryOpNode | Cast to BinaryOpNode
            if (gen_node(bc, n->left) != 0) return -1; // Генерируем левый операнд | Generate left operand
            if (gen_node(bc, n->right) != 0) return -1; // Генерируем правый операнд | Generate right operand
            
            OpCode op; // Код операции | Operation code
            if (strcmp(n->op, "+") == 0)       op = OP_ADD; // Сложение | Addition
            else if (strcmp(n->op, "-") == 0)  op = OP_SUB; // Вычитание | Subtraction
            else if (strcmp(n->op, "*") == 0)  op = OP_MUL; // Умножение | Multiplication
            else if (strcmp(n->op, "/") == 0)  op = OP_DIV; // Деление | Division
            else { // Неизвестный оператор | Unknown operator
                fprintf(stderr, "error: unknown binary operator '%s'\n", n->op); // Вывод ошибки | Print error
                return -1; // Ошибка | Error
            }
            bytecode_add(bc, op, ""); // Добавляем инструкцию операции | Add operation instruction
            return 0; // Успех | Success
        }
        
        // ----- Неизвестный узел -----
        default: // По умолчанию | Default
            fprintf(stderr, "error: codegen cannot handle node type %d\n", (int)node->type); // Вывод ошибки | Print error
            return -1; // Ошибка | Error
    }
}

// ------------------------------------------------------------
// codegen_generate — генерирует байт-код для всего блока
// ------------------------------------------------------------
Bytecode* codegen_generate(BlockNode* block) { // Генерация байт-кода для блока | Generate bytecode for block
    Bytecode* bc = bytecode_create(); // Создаём пустой байт-код | Create empty bytecode
    
    for (int i = 0; i < block->count; i++) { // Проходим по всем операторам | Loop over all statements
        if (gen_node(bc, block->statements[i]) != 0) { // Генерируем байт-код для оператора | Generate bytecode for statement
            bytecode_free(bc); // Если ошибка — освобождаем память | If error — free memory
            return NULL; // Возвращаем NULL | Return NULL
        }
    }
    
    bytecode_add(bc, OP_HALT, ""); // Добавляем HALT в конце | Add HALT at the end
    return bc; // Возвращаем готовый байт-код | Return ready bytecode
}