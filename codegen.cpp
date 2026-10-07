#include "codegen.h"
#include <stdexcept>

Bytecode CodeGen::generate(BlockNode* block) {
    for (auto& stmt : block->statements) {
        genNode(stmt.get());
    }
    bc.instructions.push_back(Instruction(OpCode::HALT, ""));
    return bc;
}

void CodeGen::genNode(ASTNode* node) {
    switch (node->type) {
        case NodeType::Number: {
            NumberNode* n = (NumberNode*)node;
            bc.instructions.push_back(Instruction(OpCode::PUSH, std::to_string(n->value)));
            return;
        }

        case NodeType::Var: {
            VarNode* n = (VarNode*)node;
            bc.instructions.push_back(Instruction(OpCode::LOAD, n->name));
            return;
        }

        case NodeType::Let: {
            LetNode* n = (LetNode*)node;
            genNode(n->value.get());
            bc.instructions.push_back(Instruction(OpCode::STORE, n->name));
            return;
        }

        case NodeType::Print: {
            PrintNode* n = (PrintNode*)node;
            genNode(n->value.get());
            bc.instructions.push_back(Instruction(OpCode::PRINT, ""));
            return;
        }

        case NodeType::BinaryOp: {
            BinaryOpNode* n = (BinaryOpNode*)node;
            genNode(n->left.get());
            genNode(n->right.get());

            if (n->op == "+")      bc.instructions.push_back(Instruction(OpCode::ADD, ""));
            else if (n->op == "-") bc.instructions.push_back(Instruction(OpCode::SUB, ""));
            else if (n->op == "*") bc.instructions.push_back(Instruction(OpCode::MUL, ""));
            else if (n->op == "/") bc.instructions.push_back(Instruction(OpCode::DIV, ""));
            else throw std::runtime_error("CodeGen: unknown operator");
            return;
        }

        default:
            throw std::runtime_error("CodeGen: unknown node type");
    }
}