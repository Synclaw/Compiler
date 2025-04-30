#pragma once
#include <string>
#include <vector>
#include <stdexcept>

struct SymbolEntry {
    std::string name;
    std::string type;
    bool isValued = false;
};

class SymbolTable {
private:
    std::vector<SymbolEntry> entries;
public:
    void addSymbol(const std::string& name, const std::string& type);
    SymbolEntry* getSymbol(const std::string& name);
    void setValued(const std::string& name);
};