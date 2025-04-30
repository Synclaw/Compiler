#include "../include/code_generator.h"
#include <iostream>

using namespace std;

void CodeGenerator::generateExpression(const ASTNode* node) {
    if (node->type == "Id") {
        out << "LOAD " << node->getAttribute("value") << ", R0\n";
    } 
    else if (node->type == "Literal") {
        out << "MOV #" << node->getAttribute("value") << ", R0\n";
    }
    else if (node->type == "BinOp") {
        generateExpression(node->children[0]);
        out << "PUSH R0\n";
        generateExpression(node->children[1]);
        out << "POP R1\n";
        
        const string op = node->getAttribute("op");
        if (op == "+") out << "ADD R1, R0\n";
        else if (op == "-") out << "SUB R1, R0\n";
        else if (op == "&&") out << "AND R1, R0\n";
        // 其他运算符处理...
    }
}

void CodeGenerator::generate(const ASTNode* node) {
    if (node->type == "Program") {
        for (const auto child : node->children) {
            generate(child);
        }
    }
    else if (node->type == "Assign") {
        generateExpression(node->children[0]);
        out << "STORE R0, " << node->getAttribute("id") << "\n";
    }
    else if (node->type == "IfStmt") {
        const string falseLabel = newLabel();
        const string exitLabel = newLabel();

        // 生成条件判断
        generateExpression(node->children[0]);
        out << "TEST R0\n";
        out << "JZ " << falseLabel << "\n";

        // 生成真分支
        generate(node->children[1]);
        out << "JMP " << exitLabel << "\n";

        // 生成假分支
        out << falseLabel << ":\n";
        if (node->children.size() > 2) {
            generate(node->children[2]);
        }

        out << exitLabel << ":\n";
    }
    else if (node->type == "WhileStmt") {
        const string loopStart = newLabel();
        const string loopEnd = newLabel();

        out << loopStart << ":\n";
        generateExpression(node->children[0]);
        out << "TEST R0\n";
        out << "JZ " << loopEnd << "\n";

        generate(node->children[1]);
        out << "JMP " << loopStart << "\n";
        out << loopEnd << ":\n";
    }
}