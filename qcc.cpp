#include "qcc.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

Bytecode qcc_compile(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto ast = parser.parse();

    CodeGen codegen;
    return codegen.generate(ast.get());
}