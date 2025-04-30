#pragma once
#include "symbol_table.h"
#include "ast_parser.h"
#include <vector>
#include <string>

class SemanticAnalyzer {
private:
    SymbolTable& symTable;
    std::vector<std::string>& errors;

    std::string processExpression(const ASTNode* node);
public:
    SemanticAnalyzer(SymbolTable& st, std::vector<std::string>& errs)
        : symTable(st), errors(errs) {}

    void analyze(const ASTNode* node);
};