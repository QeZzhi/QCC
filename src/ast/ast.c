#include "ast/ast.h"

#include <stdlib.h>

#include "common/xalloc.h"

ASTNode* ast_number_create(int value) {
    NumberNode* node = xmalloc(sizeof(NumberNode));
    node->base.type = NODE_NUMBER;
    node->value = value;
    return (ASTNode*)node;
}

ASTNode* ast_var_create(const char* name) {
    VarNode* node = xmalloc(sizeof(VarNode));
    node->base.type = NODE_VAR;
    node->name = xstrdup(name);
    return (ASTNode*)node;
}

ASTNode* ast_binary_create(const char* op, ASTNode* left, ASTNode* right) {
    BinaryOpNode* node = xmalloc(sizeof(BinaryOpNode));
    node->base.type = NODE_BINARY_OP;
    node->op = xstrdup(op);
    node->left = left;
    node->right = right;
    return (ASTNode*)node;
}

ASTNode* ast_let_create(const char* name, ASTNode* value) {
    LetNode* node = xmalloc(sizeof(LetNode));
    node->base.type = NODE_LET;
    node->name = xstrdup(name);
    node->value = value;
    return (ASTNode*)node;
}

ASTNode* ast_print_create(ASTNode* value) {
    PrintNode* node = xmalloc(sizeof(PrintNode));
    node->base.type = NODE_PRINT;
    node->value = value;
    return (ASTNode*)node;
}

BlockNode* ast_block_create(void) {
    BlockNode* block = xmalloc(sizeof(BlockNode));
    block->base.type = NODE_BLOCK;
    block->capacity = 8;
    block->count = 0;
    block->statements = xmalloc(sizeof(ASTNode*) * (size_t)block->capacity);
    return block;
}

void ast_block_add(BlockNode* block, ASTNode* stmt) {
    if (block->count >= block->capacity) {
        block->capacity *= 2;
        block->statements = xrealloc(block->statements,
                                     sizeof(ASTNode*) * (size_t)block->capacity);
    }
    block->statements[block->count++] = stmt;
}

void ast_free(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_STRING:
            free(((StringNode*)node)->value);
            break;

        case NODE_VAR:
            free(((VarNode*)node)->name);
            break;

        case NODE_BINARY_OP: {
            BinaryOpNode* n = (BinaryOpNode*)node;
            free(n->op);
            ast_free(n->left);
            ast_free(n->right);
            break;
        }

        case NODE_ASSIGN: {
            AssignNode* n = (AssignNode*)node;
            free(n->name);
            ast_free(n->value);
            break;
        }

        case NODE_LET: {
            LetNode* n = (LetNode*)node;
            free(n->name);
            ast_free(n->value);
            break;
        }

        case NODE_PRINT: {
            PrintNode* n = (PrintNode*)node;
            ast_free(n->value);
            break;
        }

        case NODE_BLOCK: {
            BlockNode* n = (BlockNode*)node;
            for (int i = 0; i < n->count; i++) {
                ast_free(n->statements[i]);
            }
            free(n->statements);
            break;
        }

        case NODE_NUMBER:
        default:
            break;
    }

    free(node);
}
