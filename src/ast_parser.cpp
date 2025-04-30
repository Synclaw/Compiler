#include "../include/ast_parser.h"
#include <fstream>
#include <sstream>
#include <stack>

using namespace std;

vector<ASTNode*> ASTParser::parseFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) throw runtime_error("无法打开文件: " + filename);

    vector<ASTNode*> rootNodes;
    stack<pair<int, ASTNode*>> nodeStack; // <缩进级别, 父节点>

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // 计算缩进层级
        int indent = 0;
        while (indent < line.size() && line[indent] == ' ') indent += 2;
        
        // 清理栈中已完成的父节点
        while (!nodeStack.empty() && nodeStack.top().first >= indent) {
            nodeStack.pop();
        }

        // 解析节点类型和属性
        istringstream iss(line.substr(indent));
        string nodeType;
        iss >> nodeType;

        ASTNode* newNode = new ASTNode{nodeType, {}, {}};

        // 处理不同节点类型的属性
        if (nodeType == "Decl") {
            string dtype, id;
            iss >> dtype >> id;
            newNode->attributes["type"] = dtype;
            newNode->attributes["id"] = id;
        } else if (nodeType == "Assign") {
            string id;
            iss >> id;
            newNode->attributes["id"] = id;
        } else if (nodeType == "BinOp") {
            string op;
            iss >> op;
            newNode->attributes["op"] = op;
        }

        // 连接父节点
        if (!nodeStack.empty()) {
            nodeStack.top().second->children.push_back(newNode);
        } else {
            rootNodes.push_back(newNode);
        }
        nodeStack.push({indent, newNode});
    }

    return rootNodes;
}

void ASTParser::cleanupAST(vector<ASTNode*>& nodes) {
    for (auto node : nodes) {
        cleanupAST(node->children); // 递归清理子节点列表
        delete node;
    }
    nodes.clear();
}