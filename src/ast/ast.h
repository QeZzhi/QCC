// ============================================================
// ast.h — Заголовочный файл для AST (абстрактного синтаксического дерева).
// Здесь описаны типы узлов, их структуры и функции для работы с ними.
// ============================================================
//
// ast.h — Header file for the AST (Abstract Syntax Tree).
// Here are described node types, their structures, and functions to work with them.
// ============================================================

#ifndef AST_H // Защита от повторного включения | Include guard
#define AST_H // Определяем макрос-страж | Define guard macro

// ------------------------------------------------------------
// NodeType — перечисление всех возможных типов узлов AST
// ------------------------------------------------------------
typedef enum { // Тип узла AST | AST node type
    NODE_NUMBER,    // Числовой литерал (например, 42) | Number literal
    NODE_STRING,    // Строковый литерал (например, "Hello") | String literal
    NODE_VAR,       // Ссылка на переменную (например, x) | Variable reference
    NODE_BINARY_OP, // Бинарная операция (например, a + b) | Binary operation
    NODE_ASSIGN,    // Присваивание (например, x = 5) | Assignment
    NODE_LET,       // Объявление переменной (например, let x = 5) | Let declaration
    NODE_PRINT,     // Оператор вывода (например, print(5)) | Print statement
    NODE_BLOCK      // Блок операторов (фигурные скобки { ... }) | Block of statements
} NodeType;

// ------------------------------------------------------------
// ASTNode — базовый узел AST (содержит только тип)
// Все остальные узлы наследуют его, добавляя свои поля.
// ------------------------------------------------------------
typedef struct ASTNode { // Базовый узел AST | Base AST node
    NodeType type;       // Тег типа узла | Node type tag
} ASTNode;

// ------------------------------------------------------------
// NumberNode — узел числового литерала
// ------------------------------------------------------------
typedef struct { // Узел-число | Number node
    ASTNode base; // Базовый узел (должен быть первым) | Base node (must be first)
    int value;    // Значение числа | Numeric value
} NumberNode;

// ------------------------------------------------------------
// StringNode — узел строкового литерала
// ------------------------------------------------------------
typedef struct { // Узел-строка | String node
    ASTNode base; // Базовый узел | Base node
    char* value;  // Указатель на текст строки | String pointer
} StringNode;

// ------------------------------------------------------------
// VarNode — узел ссылки на переменную
// ------------------------------------------------------------
typedef struct { // Узел-переменная | Variable node
    ASTNode base; // Базовый узел | Base node
    char* name;   // Имя переменной | Variable name
} VarNode;

// ------------------------------------------------------------
// BinaryOpNode — узел бинарной операции (+, -, *, /)
// ------------------------------------------------------------
typedef struct { // Узел бинарной операции | Binary operation node
    ASTNode base;   // Базовый узел | Base node
    char* op;       // Оператор (например, "+") | Operator (e.g. "+")
    ASTNode* left;  // Левый операнд | Left operand
    ASTNode* right; // Правый операнд | Right operand
} BinaryOpNode;

// ------------------------------------------------------------
// AssignNode — узел присваивания (x = ...)
// ------------------------------------------------------------
typedef struct { // Узел присваивания | Assignment node
    ASTNode base;    // Базовый узел | Base node
    char* name;      // Имя переменной | Variable name
    ASTNode* value;  // Присваиваемое значение | Assigned value
} AssignNode;

// ------------------------------------------------------------
// LetNode — узел объявления переменной (let x = ...)
// ------------------------------------------------------------
typedef struct { // Узел let-объявления | Let node
    ASTNode base;    // Базовый узел | Base node
    char* name;      // Имя переменной | Variable name
    ASTNode* value;  // Инициализирующее значение | Initial value
} LetNode;

// ------------------------------------------------------------
// PrintNode — узел вывода (print ...)
// ------------------------------------------------------------
typedef struct { // Узел вывода | Print node
    ASTNode base;    // Базовый узел | Base node
    ASTNode* value;  // Выводимое выражение | Expression to print
} PrintNode;

// ------------------------------------------------------------
// BlockNode — узел блока (несколько операторов подряд)
// ------------------------------------------------------------
typedef struct { // Узел блока | Block node
    ASTNode base;          // Базовый узел | Base node
    ASTNode** statements;  // Массив указателей на операторы | Array of statement pointers
    int count;             // Текущее число операторов | Current statement count
    int capacity;          // Выделенная ёмкость массива | Allocated array capacity
} BlockNode;

// ------------------------------------------------------------
// Функции для работы с AST (прототипы)
// ------------------------------------------------------------
ASTNode* ast_number_create(int value);      // Создать узел-число | Create number node
ASTNode* ast_string_create(const char* value); // Создать узел-строку | Create string node
ASTNode* ast_var_create(const char* name);  // Создать узел-переменную | Create variable node
ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right); // Создать узел бинарной операции | Create binary op node
ASTNode* ast_let_create(const char* name, ASTNode* value); // Создать узел let | Create let node
ASTNode* ast_print_create(ASTNode* value);  // Создать узел print | Create print node
BlockNode* ast_block_create(void);          // Создать пустой блок | Create empty block
void ast_block_add(BlockNode* block, ASTNode* stmt); // Добавить оператор в блок | Add statement to block

// Освободить узел и всё поддерево. Для NULL безопасен.
// Frees node and subtree. NULL-safe.
void ast_free(ASTNode* node); // Рекурсивно освободить узел | Recursively free node

#endif // Конец include guard | End of include guard