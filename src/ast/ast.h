#ifndef QCC_AST_H
#define QCC_AST_H

typedef enum {
    NODE_NUMBER,
    NODE_STRING,
    NODE_VAR,
    NODE_BINARY_OP,
    NODE_ASSIGN,
    NODE_LET,
    NODE_PRINT,
    NODE_BLOCK
} NodeType;

typedef struct ASTNode {
    NodeType type;
} ASTNode;

typedef struct {
    ASTNode base;
    int value;
} NumberNode;

typedef struct {
    ASTNode base;
    char* value;
} StringNode;

typedef struct {
    ASTNode base;
    char* name;
} VarNode;

typedef struct {
    ASTNode base;
    char* op;
    ASTNode* left;
    ASTNode* right;
} BinaryOpNode;

typedef struct {
    ASTNode base;
    char* name;
    ASTNode* value;
} AssignNode;

typedef struct {
    ASTNode base;
    char* name;
    ASTNode* value;
} LetNode;

typedef struct {
    ASTNode base;
    ASTNode* value;
} PrintNode;

typedef struct {
    ASTNode base;
    ASTNode** statements;
    int count;
    int capacity;
} BlockNode;

/* Функции для работы с AST */
ASTNode* ast_number_create(int value);
ASTNode* ast_var_create(const char* name);
ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right);
ASTNode* ast_let_create(const char* name, ASTNode* value);
ASTNode* ast_print_create(ASTNode* value);
BlockNode* ast_block_create(void);
void ast_block_add(BlockNode* block, ASTNode* stmt);

/* Освобождает узел и всё поддерево. Для NULL безопасен. */
void ast_free(ASTNode* node);

#endif
