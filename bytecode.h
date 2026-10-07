#pragma once
#include <string>
#include <vector>

enum class OpCode {
    PUSH,
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    PRINT,
    HALT
};

struct Instruction {
    OpCode op;
    std::string arg;

    Instruction() : op(OpCode::HALT), arg("") {}
    Instruction(OpCode o, const std::string& a) : op(o), arg(a) {}
};

struct Bytecode {
    std::vector<Instruction> instructions;
};