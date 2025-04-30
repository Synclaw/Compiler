#include <iostream>
#include <fstream>
#include <vector>
#include "ast_parser.h"
#include "semantic_analyzer.h"
#include "code_generator.h"

int main() {
    // 1. 读取输入文件
    ASTParser parser;
    std::vector<ASTNode*> ast;
    try {
        ast = parser.parseFromFile("input/parse_out.txt");
    } catch (const std::exception& e) {
        std::cerr << "错误: 无法读取语法树文件 - " << e.what() << std::endl;
        return 1;
    }

    // 2. 语义分析
    SymbolTable symTable;
    std::vector<std::string> errors;
    SemanticAnalyzer analyzer(symTable, errors);
    for (const auto* node : ast) analyzer.analyze(node);

    // 3. 结果输出
    std::ofstream outFile("output/semantic_out.txt");
    if (!errors.empty()) {
        for (const auto& err : errors) outFile << "语义错误: " << err << "\n";
        outFile << "编译终止\n";
    } else {
        CodeGenerator cg(outFile);
        for (const auto* node : ast) cg.generate(node);
        outFile << "编译成功\n";
    }

    // 4. 清理资源
    ASTParser::cleanupAST(ast);
    return 0;
}