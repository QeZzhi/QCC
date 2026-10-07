#pragma once
#include <memory>
#include "ast.h"
#include "bytecode.h"

class CodeGen {
public:
    Bytecode generate(BlockNode* block);

private:
    Bytecode bc;
    void genNode(ASTNode* node);
};