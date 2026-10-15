// ============================================================
// ast.c — реализация функций для работы с AST (деревом разбора)
// AST (Abstract Syntax Tree) — это дерево, которое представляет
// Код программы в памяти. Каждый узел — это элемент кода:
// Число, переменная, операция, объявление и т.д.
// ============================================================


// ============================================================
// ast.c — implementation of functions for working with AST (parse tree)
// AST (Abstract Syntax Tree) is a tree that represents
// Program code in memory. Each node is a code element:
// Number, variable, operation, declaration, etc
// ============================================================


#include "ast/ast.h" //Подключение заголовочного файла QCC/src/ast/ast.h | Connecting the QCC/src/ast.h header file
#include <stdlib.h> // Подлючение основной библиотеки C | Connecting the C core library
#include "common/xalloc.h" // Подключение заголовочного файла QCC/src/common/xalloc.h | Conecting the QCC/src/common/xalloc.h header file

ASTNode* ast_number_create(int value) { //Создания узла-числа | Create a number-node
    NumberNode* node = xmalloc(sizeof(NumberNode)); // Выделяем память под NumberNode | Allocate memory for the NumberNode
    node->base.type = NODE_NUMBER; //Тип узла — число | Node Type - number
    node->value = value; //Сохраняем значение (например, 31) | Store the value (e.g. 31)
    return (ASTNode*)node; // Возвращаем как ASTNode*
}

ASTNode* ast_var_create(const char* name) { // Создание узла-переменной | Create a variable-node
    VarNode* node = xmalloc(sizeof(VarNode)); // Выделяем память под VarNode | Allocate memory for VarNode
    node->base.type = NODE_VAR; // Тип узла — переменная | Node type — variable
    node->name = xstrdup(name); // Копируем имя переменной | Copy variable name
    return (ASTNode*)node; // Возвращаем как ASTNode* | Return as ASTNode*
}

ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right) { // Создание узла-операции | Create a binary operation node
    BinaryOpNode* node = xmalloc(sizeof(BinaryOpNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_BINARY_OP; // Тип узла — операция | Node type — operation
    node->op = xstrdup(op); // Копируем оператор (+, -, *, /) | Copy operator
    node->left = left; // Левый операнд | Left operand
    node->right = right; // Правый операнд | Right operand
    return (ASTNode*)node; // Возвращаем как ASTNode* | Return as ASTNode*
}

ASTNode* ast_let_create(const char* name, ASTNode* value) { // Создание узла let | Create a let-node
    LetNode* node = xmalloc(sizeof(LetNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_LET; // Тип узла — let | Node type — let
    node->name = xstrdup(name); // Копируем имя переменной | Copy variable name
    node->value = value; // Выражение справа | Right expression
    return (ASTNode*)node; // Возвращаем как ASTNode* | Return as ASTNode*
}

ASTNode* ast_print_create(ASTNode* value) { // Создание узла print | Create a print-node
    PrintNode* node = xmalloc(sizeof(PrintNode)); // Выделяем память | Allocate memory
    node->base.type = NODE_PRINT; // Тип узла — print | Node type — print
    node->value = value; // Что выводить | What to print
    return (ASTNode*)node; // Возвращаем как ASTNode* | Return as ASTNode*
}

BlockNode* ast_block_create(void) { // Создание пустого блока | Create an empty block
    BlockNode* block = xmalloc(sizeof(BlockNode)); // Выделяем память | Allocate memory
    block->base.type = NODE_BLOCK; // Тип узла — блок | Node type — block
    block->capacity = 8; // Начальная ёмкость массива | Initial array capacity
    block->count = 0; // Сколько операторов добавлено | Number of statements added
    block->statements = xmalloc(sizeof(ASTNode*) * (size_t)block->capacity); // Массив операторов | Statements array
    return block; // Возвращаем блок | Return block
}

void ast_block_add(BlockNode* block, ASTNode* stmt) { // Добавить оператор в блок | Add statement to block
    if (block->count >= block->capacity) { // Если места нет | If no space left
        block->capacity *= 2; // Увеличиваем ёмкость в 2 раза | Double the capacity
        block->statements = xrealloc(block->statements,
                                     sizeof(ASTNode*) * (size_t)block->capacity); // Расширяем массив | Resize array
    }
    block->statements[block->count++] = stmt; // Добавляем оператор в конец | Add statement to the end
}

void ast_free(ASTNode* node) { // Освобождение памяти дерева AST | Free AST tree memory
    if (!node) return; // Защита от NULL | NULL protection

    switch (node->type) { // Смотрим тип узла | Check node type
        case NODE_STRING: // Строка | String
            free(((StringNode*)node)->value); // Освобождаем строку | Free string
            break;

        case NODE_VAR: // Переменная | Variable
            free(((VarNode*)node)->name); // Освобождаем имя | Free name
            break;

        case NODE_BINARY_OP: { // Операция | Operation
            BinaryOpNode* n = (BinaryOpNode*)node;
            free(n->op); // Освобождаем оператор | Free operator
            ast_free(n->left); // Рекурсивно левый операнд | Recursively free left
            ast_free(n->right); // Рекурсивно правый операнд | Recursively free right
            break;
        }

        case NODE_ASSIGN: { // Присваивание | Assignment
            AssignNode* n = (AssignNode*)node;
            free(n->name); // Освобождаем имя | Free name
            ast_free(n->value); // Рекурсивно значение | Recursively free value
            break;
        }

        case NODE_LET: { // Let | Let
            LetNode* n = (LetNode*)node;
            free(n->name); // Освобождаем имя | Free name
            ast_free(n->value); // Рекурсивно значение | Recursively free value
            break;
        }

        case NODE_PRINT: { // Print | Print
            PrintNode* n = (PrintNode*)node;
            ast_free(n->value); // Рекурсивно значение | Recursively free value
            break;
        }

        case NODE_BLOCK: { // Блок | Block
            BlockNode* n = (BlockNode*)node;
            for (int i = 0; i < n->count; i++) { // Проходим по всем операторам | Loop over all statements
                ast_free(n->statements[i]); // Освобождаем каждый | Free each one
            }
            free(n->statements); // Освобождаем массив | Free array
            break;
        }

        case NODE_NUMBER: // Число | Number
        default:
            break; // Нечего освобождать | Nothing to free
    }

    free(node); // Освобождаем сам узел | Free the node itself
}