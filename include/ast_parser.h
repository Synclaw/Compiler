#pragma once
#include <vector>
#include <map>
#include <string>

struct ASTNode {
    std::string type;
    std::map<std::string, std::string> attributes;
    std::vector<ASTNode*> children;

    std::string getAttribute(const std::string& key) const {
        return attributes.count(key) ? attributes.at(key) : "";
    }
};

class ASTParser {
public:
    std::vector<ASTNode*> parseFromFile(const std::string& filename);
    static void cleanupAST(std::vector<ASTNode*>& nodes);
};