// ============================================================
// ast.h — Заголовочный файл для AST (абстрактного синтаксического дерева).
// Здесь описаны:
//  - типы узлов AST (NodeType);
//  - структуры конкретных узлов (NumberNode, BinaryOpNode и т.д.);
//  - прототипы функций для создания и освобождения узлов.
// ============================================================


// ============================================================
// ast.h — Header file for the AST (Abstract Syntax Tree).
// Here are described:
//  - AST node types (NodeType);
//  - concrete node structures (NumberNode, BinaryOpNode, etc.);
//  - prototypes for creating and freeing nodes.
// ============================================================


#ifndef AST_H // Защита от повторного включения | Include guard
#define AST_H // Определяем макрос-страж | Define guard macro

typedef enum { // Тип узла AST | AST node type
    NODE_NUMBER, // Числовой литерал | Number literal
    NODE_STRING, // Строковый литерал | String literal
    NODE_VAR, // Ссылка на переменную | Variable reference
    NODE_BINARY_OP, // Бинарная операция | Binary operation
    NODE_ASSIGN, // Присваивание | Assignment
    NODE_LET, // Объявление переменной (let) | Let declaration
    NODE_PRINT, // Оператор вывода (print) | Print statement
    NODE_BLOCK // Блок операторов | Block of statements
} NodeType; // Псевдоним типа для enum | Type alias for the enum

typedef struct ASTNode { // Базовый узел AST | Base AST node
    NodeType type; // Имя типа | Type name
} ASTNode; 

typedef struct { // Узел-число | Number node
    ASTNode base; // Базовый узел (должен быть первым) | Base node (must be first)
    int value; // Значение числа | Numeric value
} NumberNode; // Имя типа структуры | Struct type name

typedef struct { // Узел-строка | String node
    ASTNode base; // Базовый узел | Base node
    char* value; // Указатель на строку | String pointer
} StringNode; // Имя типа структуры | Struct type name

typedef struct { // Узел-переменная | Variable node
    ASTNode base; // Базовый узел | Base node
    char* name; // Имя переменной | Variable name
} VarNode; // Имя типа структуры | Struct type name

typedef struct { // Узел бинарной операции | Binary operation node
    ASTNode base; // Базовый узел | Base node
    char* op; // Оператор (например, "+") | Operator (e.g. "+")
    ASTNode* left; // Левый операнд | Left operand
    ASTNode* right; // Правый операнд | Right operand
} BinaryOpNode; // Имя типа структуры | Struct type name

typedef struct { // Узел присваивания | Assignment node
    ASTNode base; // Базовый узел | Base node
    char* name; // Имя переменной | Variable name
    ASTNode* value; // Присваиваемое значение | Assigned value
} AssignNode; // Имя типа структуры | Struct type name

typedef struct { // Узел let-объявления | Let node
    ASTNode base; // Базовый узел | Base node
    char* name; // Имя переменной | Variable name
    ASTNode* value; // Инициализирующее значение | Initial value
} LetNode; // Имя типа структуры | Struct type name

typedef struct { // Узел вывода | Print node
    ASTNode base; // Базовый узел | Base node
    ASTNode* value; // Выводимое выражение | Expression to print
} PrintNode; // Имя типа структуры | Struct type name

typedef struct { // Узел блока | Block node
    ASTNode base; // Базовый узел | Base node
    ASTNode** statements; // Массив указателей на операторы | Array of statement pointers
    int count; // Текущее число операторов | Current statement count
    int capacity; // Выделенная ёмкость массива | Allocated array capacity
} BlockNode; // Имя типа структуры | Struct type name

// Функции для работы с AST | AST API 
ASTNode* ast_number_create(int value); // Создать узел-число | Create number node
ASTNode* ast_var_create(const char* name); // Создать узел-переменную | Create variable node
ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right); // Создать бинарную операцию | Create binary op node
ASTNode* ast_let_create(const char* name, ASTNode* value); // Создать let-узел | Create let node
ASTNode* ast_print_create(ASTNode* value); // Создать узел вывода | Create print node
BlockNode* ast_block_create(void); // Создать пустой блок | Create empty block
void ast_block_add(BlockNode* block, ASTNode* stmt); // Добавить оператор в блок | Add statement to block

// Освобождает узел и всё поддерево. Для NULL безопасен
// Frees node and subtree. NULL-safe
void ast_free(ASTNode* node); // Рекурсивно освободить узел | Recursively free node

#endif // Конец include guard | End of include guard