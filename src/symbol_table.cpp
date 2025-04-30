#include "../include/symbol_table.h"

void SymbolTable::addSymbol(const std::string& name, const std::string& type) {
    for (const auto& entry : entries) {
        if (entry.name == name) {
            throw std::runtime_error("重复声明变量: " + name);
        }
    }
    entries.push_back({name, type, false});
}

SymbolEntry* SymbolTable::getSymbol(const std::string& name) {
    for (auto& entry : entries) {
        if (entry.name == name) return &entry;
    }
    return nullptr;
}

void SymbolTable::setValued(const std::string& name) {
    if (auto* entry = getSymbol(name)) entry->isValued = true;
}