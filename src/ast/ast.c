// ============================================================
// ast.c — реализация функций для работы с AST (деревом разбора)
// AST (Abstract Syntax Tree) — это дерево, которое представляет
// Код программы в памяти. Каждый узел — это элемент кода:
// Число, переменная, операция, объявление и т.д.
// ============================================================
//
// ast.c — implementation of functions for working with AST (parse tree)
// AST (Abstract Syntax Tree) is a tree that represents
// Program code in memory. Each node is a code element:
// Number, variable, operation, declaration, etc
// ============================================================

#include "ast.h"        // Подключение заголовочного файла QCC/src/ast/ast.h | Connecting the QCC/src/ast/ast.h header file
#include <stdlib.h>         // Подключение основной библиотеки C для malloc/free | Connecting the C core library for malloc/free
#include "../common/xalloc.h"  // Подключение безопасных аллокаторов (xmalloc, xstrdup) | Connecting safe allocators (xmalloc, xstrdup)

// ------------------------------------------------------------
// ast_string_create — создать узел строкового литерала
// Вход: value — текст строки (например, "Hello")
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_string_create(const char* value) {
    StringNode* node = xmalloc(sizeof(StringNode)); // Выделяем память под структуру StringNode | Allocate memory for StringNode struct
    node->base.type = NODE_STRING;                  // Устанавливаем тип узла — строка | Set node type — string
    node->value = xstrdup(value);                   // Копируем строку в память узла | Copy string into node memory
    return (ASTNode*)node;                          // Возвращаем как базовый указатель ASTNode* | Return as base ASTNode* pointer
}

// ------------------------------------------------------------
// ast_number_create — создать узел числа
// Вход: value — целое число (например, 42)
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_number_create(int value) {
    NumberNode* node = xmalloc(sizeof(NumberNode)); // Выделяем память под NumberNode | Allocate memory for NumberNode
    node->base.type = NODE_NUMBER;                  // Тип узла — число | Node type — number
    node->value = value;                            // Сохраняем само значение | Store the actual value
    return (ASTNode*)node;                          // Возвращаем как ASTNode* | Return as ASTNode*
}

// ------------------------------------------------------------
// ast_var_create — создать узел ссылки на переменную
// Вход: name — имя переменной (например, "x")
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_var_create(const char* name) {
    VarNode* node = xmalloc(sizeof(VarNode)); // Выделяем память под VarNode | Allocate memory for VarNode
    node->base.type = NODE_VAR;               // Тип узла — переменная | Node type — variable
    node->name = xstrdup(name);               // Копируем имя переменной | Copy variable name
    return (ASTNode*)node;                    // Возвращаем как ASTNode* | Return as ASTNode*
}

// ------------------------------------------------------------
// ast_binary_create — создать узел бинарной операции
// Вход: op — оператор ("+", "-", "*", "/"), left и right — операнды
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right) {
    BinaryOpNode* node = xmalloc(sizeof(BinaryOpNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_BINARY_OP;                   // Тип узла — операция | Node type — operation
    node->op = xstrdup(op);                             // Копируем символ оператора | Copy operator symbol
    node->left = left;                                  // Сохраняем левый операнд | Store left operand
    node->right = right;                                // Сохраняем правый операнд | Store right operand
    return (ASTNode*)node;                              // Возвращаем как ASTNode* | Return as ASTNode*
}

// ------------------------------------------------------------
// ast_let_create — создать узел объявления переменной (let)
// Вход: name — имя переменной, value — выражение для присвоения
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_let_create(const char* name, ASTNode* value) {
    LetNode* node = xmalloc(sizeof(LetNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_LET;               // Тип узла — let | Node type — let
    node->name = xstrdup(name);               // Копируем имя переменной | Copy variable name
    node->value = value;                      // Сохраняем выражение справа | Store right-hand expression
    return (ASTNode*)node;                    // Возвращаем как ASTNode* | Return as ASTNode*
}

// ------------------------------------------------------------
// ast_print_create — создать узел вывода (print)
// Вход: value — выражение, которое нужно вывести
// Выход: указатель на созданный узел
// ------------------------------------------------------------
ASTNode* ast_print_create(ASTNode* value) {
    PrintNode* node = xmalloc(sizeof(PrintNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_PRINT;                 // Тип узла — print | Node type — print
    node->value = value;                          // Сохраняем выводимое выражение | Store expression to print
    return (ASTNode*)node;                        // Возвращаем как ASTNode* | Return as ASTNode*
}

// ------------------------------------------------------------
// ast_block_create — создать пустой блок операторов
// Выход: указатель на созданный BlockNode
// ------------------------------------------------------------
BlockNode* ast_block_create(void) {
    BlockNode* block = xmalloc(sizeof(BlockNode)); // Выделяем память | Allocate memory
    block->base.type = NODE_BLOCK;                 // Тип узла — блок | Node type — block
    block->capacity = 8;                           // Начальная ёмкость массива | Initial array capacity
    block->count = 0;                              // Пока нет ни одного оператора | No statements yet
    block->statements = xmalloc(sizeof(ASTNode*) * (size_t)block->capacity); // Выделяем массив под операторы | Allocate array for statements
    return block;                                  // Возвращаем блок | Return block
}

// ------------------------------------------------------------
// ast_block_add — добавить оператор в блок
// Вход: block — блок, stmt — добавляемый оператор
// ------------------------------------------------------------
void ast_block_add(BlockNode* block, ASTNode* stmt) {
    if (block->count >= block->capacity) { // Если места в массиве нет | If no space in array
        block->capacity *= 2;              // Увеличиваем ёмкость в 2 раза | Double the capacity
        block->statements = xrealloc(block->statements,
                                     sizeof(ASTNode*) * (size_t)block->capacity); // Расширяем массив | Resize array
    }
    block->statements[block->count++] = stmt; // Добавляем оператор в конец | Add statement to the end
}

// ------------------------------------------------------------
// ast_free — рекурсивно освободить память узла и всех его потомков
// Вход: node — узел, который нужно удалить
// ------------------------------------------------------------
void ast_free(ASTNode* node) {
    if (!node) return; // Защита от NULL | NULL protection
    
    switch (node->type) {
        case NODE_STRING: // Строковый литерал | String literal
            free(((StringNode*)node)->value); // Освобождаем текст строки | Free the string text
            break;
        
        case NODE_VAR: // Переменная | Variable
            free(((VarNode*)node)->name); // Освобождаем имя переменной | Free variable name
            break;
        
        case NODE_BINARY_OP: { // Бинарная операция | Binary operation
            BinaryOpNode* n = (BinaryOpNode*)node;
            free(n->op);          // Освобождаем символ оператора | Free operator symbol
            ast_free(n->left);    // Рекурсивно освобождаем левый операнд | Recursively free left operand
            ast_free(n->right);   // Рекурсивно освобождаем правый операнд | Recursively free right operand
            break;
        }
        
        case NODE_ASSIGN: { // Присваивание | Assignment
            AssignNode* n = (AssignNode*)node;
            free(n->name);        // Освобождаем имя | Free name
            ast_free(n->value);   // Рекурсивно освобождаем значение | Recursively free value
            break;
        }
        
        case NODE_LET: { // Объявление переменной (let) | Let declaration
            LetNode* n = (LetNode*)node;
            free(n->name);        // Освобождаем имя | Free name
            ast_free(n->value);   // Рекурсивно освобождаем значение | Recursively free value
            break;
        }
        
        case NODE_PRINT: { // Вывод | Print
            PrintNode* n = (PrintNode*)node;
            ast_free(n->value);   // Рекурсивно освобождаем выводимое выражение | Recursively free printed expression
            break;
        }
        
        case NODE_BLOCK: { // Блок операторов | Block of statements
            BlockNode* n = (BlockNode*)node;
            for (int i = 0; i < n->count; i++) {
                ast_free(n->statements[i]); // Освобождаем каждый оператор | Free each statement
            }
            free(n->statements); // Освобождаем сам массив | Free the array itself
            break;
        }
        
        case NODE_NUMBER: // Число | Number
        default:          // Остальные узлы не требуют дополнительной очистки | Other nodes require no extra cleanup
            break;
    }
    
    free(node); // Освобождаем сам узел | Free the node itself
}