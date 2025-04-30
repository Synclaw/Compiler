#include "../include/semantic_analyzer.h"
#include "../include/symbol_table.h"

using namespace std;

string SemanticAnalyzer::processExpression(const ASTNode* node) {
    if (node->type == "Id") {
        const string id = node->getAttribute("value");
        SymbolEntry* entry = symTable.getSymbol(id);
        if (!entry) {
            errors.push_back("未声明的变量: " + id);
            return "error";
        }
        if (!entry->isValued) {
            errors.push_back("变量未初始化: " + id);
            return "error";
        }
        return entry->type;
    } 
    else if (node->type == "Literal") {
        return "int"; // 假设所有字面量都是整型
    }
    else if (node->type == "BinOp") {
        const string op = node->getAttribute("op");
        const string ltype = processExpression(node->children[0]);
        const string rtype = processExpression(node->children[1]);

        if (ltype == "error" || rtype == "error") return "error";

        // 类型检查规则
        if (op == "+" || op == "-" || op == "*" || op == "/") {
            if (ltype != "int" || rtype != "int") {
                errors.push_back("算术运算类型不匹配: " + ltype + " " + op + " " + rtype);
                return "error";
            }
            return "int";
        } 
        else if (op == "==" || op == "!=") {
            if (ltype != rtype) {
                errors.push_back("比较运算类型不兼容: " + ltype + " vs " + rtype);
                return "error";
            }
            return "bool";
        }
    }
    return "error";
}

void SemanticAnalyzer::analyze(const ASTNode* node) {
    if (node->type == "Decl") { // 变量声明处理
        try {
            symTable.addSymbol(
                node->getAttribute("id"),
                node->getAttribute("type")
            );
        } catch (const exception& e) {
            errors.push_back(e.what());
        }
    } 
    else if (node->type == "Assign") { // 赋值语句处理
        const string id = node->getAttribute("id");
        SymbolEntry* entry = symTable.getSymbol(id);
        if (!entry) {
            errors.push_back("赋值给未声明变量: " + id);
            return;
        }

        const string exprType = processExpression(node->children[0]);
        if (exprType != entry->type) {
            errors.push_back("类型不匹配: " + id + " 期望 " + entry->type + " 实际 " + exprType);
        }
        symTable.setValued(id);
    }
    else if (node->type == "IfStmt" || node->type == "WhileStmt") { // 控制流处理
        const string condType = processExpression(node->children[0]);
        if (condType != "bool") {
            errors.push_back("条件表达式必须是布尔型");
        }
    }

    // 递归处理子节点
    for (const auto child : node->children) {
        analyze(child);
    }
}