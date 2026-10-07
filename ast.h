#pragma once
#include <string>
#include <vector>
#include <memory>

enum class NodeType {
    Number, String, Var, BinaryOp, Assign, Let, Print, Block
};

struct ASTNode {
    NodeType type;
    virtual ~ASTNode() = default;
    ASTNode(NodeType t) : type(t) {}
};

struct NumberNode : ASTNode {
    int value;
    NumberNode() : ASTNode(NodeType::Number) {}
};

struct StringNode : ASTNode {
    std::string value;
    StringNode() : ASTNode(NodeType::String) {}
};

struct VarNode : ASTNode {
    std::string name;
    VarNode() : ASTNode(NodeType::Var) {}
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOpNode() : ASTNode(NodeType::BinaryOp) {}
};

struct AssignNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    AssignNode() : ASTNode(NodeType::Assign) {}
};

struct LetNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    LetNode() : ASTNode(NodeType::Let) {}
};

struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> value;
    PrintNode() : ASTNode(NodeType::Print) {}
};

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockNode() : ASTNode(NodeType::Block) {}
};