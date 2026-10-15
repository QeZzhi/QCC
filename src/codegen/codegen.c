#include "codegen/codegen.h"

#include <stdio.h>
#include <string.h>

/* 0 — успех, -1 — ошибка (сообщение уже напечатано). */
static int gen_node(Bytecode* bc, const ASTNode* node) {
    switch (node->type) {
        case NODE_NUMBER: {
            const NumberNode* n = (const NumberNode*)node;
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", n->value);
            bytecode_add(bc, OP_PUSH, buf);
            return 0;
        }

        case NODE_VAR: {
            const VarNode* n = (const VarNode*)node;
            bytecode_add(bc, OP_LOAD, n->name);
            return 0;
        }

        case NODE_LET: {
            const LetNode* n = (const LetNode*)node;
            if (gen_node(bc, n->value) != 0) return -1;
            bytecode_add(bc, OP_STORE, n->name);
            return 0;
        }

        case NODE_PRINT: {
            const PrintNode* n = (const PrintNode*)node;
            if (gen_node(bc, n->value) != 0) return -1;
            bytecode_add(bc, OP_PRINT, "");
            return 0;
        }

        case NODE_BINARY_OP: {
            const BinaryOpNode* n = (const BinaryOpNode*)node;
            if (gen_node(bc, n->left) != 0) return -1;
            if (gen_node(bc, n->right) != 0) return -1;

            OpCode op;
            if (strcmp(n->op, "+") == 0)       op = OP_ADD;
            else if (strcmp(n->op, "-") == 0)  op = OP_SUB;
            else if (strcmp(n->op, "*") == 0)  op = OP_MUL;
            else if (strcmp(n->op, "/") == 0)  op = OP_DIV;
            else {
                fprintf(stderr, "error: unknown binary operator '%s'\n", n->op);
                return -1;
            }
            bytecode_add(bc, op, "");
            return 0;
        }

        default:
            fprintf(stderr, "error: codegen cannot handle node type %d\n", (int)node->type);
            return -1;
    }
}

Bytecode* codegen_generate(BlockNode* block) {
    Bytecode* bc = bytecode_create();

    for (int i = 0; i < block->count; i++) {
        if (gen_node(bc, block->statements[i]) != 0) {
            bytecode_free(bc);
            return NULL;
        }
    }

    bytecode_add(bc, OP_HALT, "");
    return bc;
}
