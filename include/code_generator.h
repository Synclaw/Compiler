#pragma once
#include "ast_parser.h"
#include <fstream>
#include <string>

class CodeGenerator {
private:
    std::ofstream& out;
    int labelCounter = 0;

    std::string newLabel() {
        return "L" + std::to_string(labelCounter++);
    }

    void generateExpression(const ASTNode* node);
public:
    CodeGenerator(std::ofstream& o) : out(o) {}

    void generate(const ASTNode* node);
};