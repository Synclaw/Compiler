# LittleC Compiler - Usage Examples and Tutorials

## Table of Contents

1. [Basic Usage Examples](#basic-usage-examples)
2. [Step-by-Step Tutorials](#step-by-step-tutorials)
3. [Advanced Integration Patterns](#advanced-integration-patterns)
4. [Error Handling Examples](#error-handling-examples)
5. [Custom Extensions](#custom-extensions)
6. [Performance Optimization](#performance-optimization)
7. [Testing Patterns](#testing-patterns)

---

## Basic Usage Examples

### Example 1: Simple Variable Declaration and Assignment

**Input AST File (`input/simple.txt`):**
```
Program
  Decl int x
  Assign x
    Literal 42
```

**Complete Processing Code:**
```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "ast_parser.h"
#include "semantic_analyzer.h"
#include "code_generator.h"

int main() {
    // Parse AST
    ASTParser parser;
    std::vector<ASTNode*> ast = parser.parseFromFile("input/simple.txt");
    
    // Semantic analysis
    SymbolTable symTable;
    std::vector<std::string> errors;
    SemanticAnalyzer analyzer(symTable, errors);
    
    for (const auto* node : ast) {
        analyzer.analyze(node);
    }
    
    // Code generation
    std::ofstream outFile("output/simple_output.txt");
    if (errors.empty()) {
        CodeGenerator generator(outFile);
        for (const auto* node : ast) {
            generator.generate(node);
        }
        outFile << "Compilation successful\n";
    } else {
        for (const auto& error : errors) {
            outFile << "Error: " << error << "\n";
        }
    }
    
    // Cleanup
    ASTParser::cleanupAST(ast);
    return 0;
}
```

**Expected Output (`output/simple_output.txt`):**
```
MOV #42, R0
STORE R0, x
Compilation successful
```

### Example 2: Arithmetic Expression

**Input AST File (`input/arithmetic.txt`):**
```
Program
  Decl int x
  Decl int y
  Decl int result
  Assign x
    Literal 10
  Assign y
    Literal 5
  Assign result
    BinOp +
      Id x
      Id y
```

**Expected Assembly Output:**
```
MOV #10, R0
STORE R0, x
MOV #5, R0
STORE R0, y
LOAD x, R0
PUSH R0
LOAD y, R0
POP R1
ADD R1, R0
STORE R0, result
Compilation successful
```

### Example 3: Conditional Statement

**Input AST File (`input/conditional.txt`):**
```
Program
  Decl bool flag
  Assign flag
    Literal true
  IfStmt
    Id flag
    Assign x
      Literal 100
```

**Expected Assembly Output:**
```
MOV #true, R0
STORE R0, flag
LOAD flag, R0
TEST R0
JZ L0
MOV #100, R0
STORE R0, x
L0:
Compilation successful
```

---

## Step-by-Step Tutorials

### Tutorial 1: Building a Custom Compiler Driver

**Step 1: Create the Driver Class**

```cpp
// compiler_driver.h
#pragma once
#include <string>
#include <vector>
#include "ast_parser.h"
#include "semantic_analyzer.h"
#include "code_generator.h"

class CompilerDriver {
private:
    std::string inputPath;
    std::string outputPath;
    bool debugMode;
    
public:
    CompilerDriver(const std::string& input, const std::string& output, bool debug = false);
    
    bool compile();
    void printSymbolTable(const SymbolTable& symTable);
    void printErrors(const std::vector<std::string>& errors);
};
```

**Step 2: Implement the Driver**

```cpp
// compiler_driver.cpp
#include "compiler_driver.h"
#include <iostream>
#include <fstream>

CompilerDriver::CompilerDriver(const std::string& input, const std::string& output, bool debug)
    : inputPath(input), outputPath(output), debugMode(debug) {}

bool CompilerDriver::compile() {
    try {
        // Phase 1: Parse AST
        if (debugMode) std::cout << "Phase 1: Parsing AST...\n";
        ASTParser parser;
        std::vector<ASTNode*> ast = parser.parseFromFile(inputPath);
        
        // Phase 2: Semantic Analysis
        if (debugMode) std::cout << "Phase 2: Semantic analysis...\n";
        SymbolTable symTable;
        std::vector<std::string> errors;
        SemanticAnalyzer analyzer(symTable, errors);
        
        for (const auto* node : ast) {
            analyzer.analyze(node);
        }
        
        if (debugMode) {
            printSymbolTable(symTable);
            if (!errors.empty()) printErrors(errors);
        }
        
        // Phase 3: Code Generation
        std::ofstream outFile(outputPath);
        if (errors.empty()) {
            if (debugMode) std::cout << "Phase 3: Code generation...\n";
            CodeGenerator generator(outFile);
            for (const auto* node : ast) {
                generator.generate(node);
            }
            outFile << "Compilation successful\n";
            std::cout << "Compilation completed successfully\n";
        } else {
            for (const auto& error : errors) {
                outFile << "Semantic error: " << error << "\n";
            }
            outFile << "Compilation terminated\n";
            std::cout << "Compilation failed with " << errors.size() << " errors\n";
        }
        
        // Cleanup
        ASTParser::cleanupAST(ast);
        return errors.empty();
        
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return false;
    }
}

void CompilerDriver::printSymbolTable(const SymbolTable& symTable) {
    std::cout << "\n=== Symbol Table ===\n";
    // Note: This requires adding a getter method to SymbolTable
    // or making entries public for debugging
}

void CompilerDriver::printErrors(const std::vector<std::string>& errors) {
    std::cout << "\n=== Semantic Errors ===\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        std::cout << "[" << (i+1) << "] " << errors[i] << "\n";
    }
}
```

**Step 3: Usage Example**

```cpp
// main.cpp
#include "compiler_driver.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }
    
    CompilerDriver compiler(argv[1], argv[2], true); // Enable debug mode
    return compiler.compile() ? 0 : 1;
}
```

### Tutorial 2: Processing Multiple Files

**Batch Compiler Implementation:**

```cpp
#include <filesystem>
#include <iostream>
#include <vector>
#include "compiler_driver.h"

class BatchCompiler {
private:
    std::string inputDir;
    std::string outputDir;
    
public:
    BatchCompiler(const std::string& inDir, const std::string& outDir)
        : inputDir(inDir), outputDir(outDir) {}
    
    void compileAll() {
        namespace fs = std::filesystem;
        
        if (!fs::exists(outputDir)) {
            fs::create_directories(outputDir);
        }
        
        int successCount = 0;
        int totalCount = 0;
        
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (entry.path().extension() == ".txt") {
                std::string inputFile = entry.path().string();
                std::string outputFile = outputDir + "/" + 
                    entry.path().stem().string() + "_output.txt";
                
                std::cout << "Compiling: " << inputFile << " -> " << outputFile << "\n";
                
                CompilerDriver compiler(inputFile, outputFile);
                if (compiler.compile()) {
                    successCount++;
                }
                totalCount++;
            }
        }
        
        std::cout << "\nBatch compilation completed: " 
                  << successCount << "/" << totalCount << " files successful\n";
    }
};

int main() {
    BatchCompiler batch("input/", "output/");
    batch.compileAll();
    return 0;
}
```

---

## Advanced Integration Patterns

### Pattern 1: AST Visitor for Custom Analysis

```cpp
// ast_visitor.h
#pragma once
#include "ast_parser.h"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(const ASTNode* node) {
        // Pre-order traversal
        visitNode(node);
        for (const auto* child : node->children) {
            visit(child);
        }
    }
    
protected:
    virtual void visitNode(const ASTNode* node) = 0;
};

// Example: Variable usage counter
class VariableUsageCounter : public ASTVisitor {
private:
    std::map<std::string, int> usageCounts;
    
protected:
    void visitNode(const ASTNode* node) override {
        if (node->type == "Id") {
            std::string varName = node->getAttribute("value");
            usageCounts[varName]++;
        }
    }
    
public:
    void printUsageStats() {
        std::cout << "\n=== Variable Usage Statistics ===\n";
        for (const auto& pair : usageCounts) {
            std::cout << pair.first << ": " << pair.second << " uses\n";
        }
    }
};
```

### Pattern 2: Custom Symbol Table with Scoping

```cpp
// scoped_symbol_table.h
#pragma once
#include "symbol_table.h"
#include <stack>

class ScopedSymbolTable {
private:
    std::stack<SymbolTable> scopes;
    
public:
    ScopedSymbolTable() {
        enterScope(); // Global scope
    }
    
    void enterScope() {
        scopes.push(SymbolTable());
    }
    
    void exitScope() {
        if (scopes.size() > 1) { // Keep global scope
            scopes.pop();
        }
    }
    
    void addSymbol(const std::string& name, const std::string& type) {
        scopes.top().addSymbol(name, type);
    }
    
    SymbolEntry* getSymbol(const std::string& name) {
        // Search from current scope to global
        std::stack<SymbolTable> tempStack = scopes;
        while (!tempStack.empty()) {
            SymbolEntry* entry = tempStack.top().getSymbol(name);
            if (entry) return entry;
            tempStack.pop();
        }
        return nullptr;
    }
    
    void setValued(const std::string& name) {
        SymbolEntry* entry = getSymbol(name);
        if (entry) {
            // This requires modification to SymbolTable to support this pattern
            // or we need a different approach
        }
    }
};
```

### Pattern 3: AST Transformation Pipeline

```cpp
// ast_transformer.h
#pragma once
#include "ast_parser.h"

class ASTTransformer {
public:
    virtual ~ASTTransformer() = default;
    virtual ASTNode* transform(ASTNode* node) = 0;
};

// Example: Constant folding transformer
class ConstantFoldingTransformer : public ASTTransformer {
public:
    ASTNode* transform(ASTNode* node) override {
        // Transform children first
        for (auto& child : node->children) {
            child = transform(child);
        }
        
        // Apply constant folding to binary operations
        if (node->type == "BinOp" && node->children.size() == 2) {
            auto* left = node->children[0];
            auto* right = node->children[1];
            
            if (left->type == "Literal" && right->type == "Literal") {
                return foldConstants(node, left, right);
            }
        }
        
        return node;
    }
    
private:
    ASTNode* foldConstants(ASTNode* binOp, ASTNode* left, ASTNode* right) {
        std::string op = binOp->getAttribute("op");
        int leftVal = std::stoi(left->getAttribute("value"));
        int rightVal = std::stoi(right->getAttribute("value"));
        
        int result;
        if (op == "+") result = leftVal + rightVal;
        else if (op == "-") result = leftVal - rightVal;
        else if (op == "*") result = leftVal * rightVal;
        else if (op == "/") result = leftVal / rightVal;
        else return binOp; // Not foldable
        
        // Create new literal node
        ASTNode* newNode = new ASTNode{
            "Literal", 
            {{"value", std::to_string(result)}}, 
            {}
        };
        
        // Clean up old nodes
        delete left;
        delete right;
        delete binOp;
        
        return newNode;
    }
};

// Usage example
void applyTransformations(std::vector<ASTNode*>& ast) {
    ConstantFoldingTransformer folder;
    
    for (auto& node : ast) {
        node = folder.transform(node);
    }
}
```

---

## Error Handling Examples

### Example 1: Comprehensive Error Collection

```cpp
// error_manager.h
#pragma once
#include <vector>
#include <string>

enum class ErrorType {
    PARSE_ERROR,
    SEMANTIC_ERROR,
    CODE_GEN_ERROR
};

struct CompilerError {
    ErrorType type;
    std::string message;
    std::string location;
    int severity; // 1=warning, 2=error, 3=fatal
};

class ErrorManager {
private:
    std::vector<CompilerError> errors;
    
public:
    void addError(ErrorType type, const std::string& message, 
                  const std::string& location = "", int severity = 2) {
        errors.push_back({type, message, location, severity});
    }
    
    bool hasErrors() const {
        return std::any_of(errors.begin(), errors.end(),
            [](const CompilerError& e) { return e.severity >= 2; });
    }
    
    bool hasFatalErrors() const {
        return std::any_of(errors.begin(), errors.end(),
            [](const CompilerError& e) { return e.severity >= 3; });
    }
    
    void printErrors() const {
        for (const auto& error : errors) {
            std::string typeStr = (error.type == ErrorType::PARSE_ERROR) ? "Parse" :
                                 (error.type == ErrorType::SEMANTIC_ERROR) ? "Semantic" : "CodeGen";
            std::string severityStr = (error.severity == 1) ? "Warning" :
                                     (error.severity == 2) ? "Error" : "Fatal";
            
            std::cout << "[" << typeStr << " " << severityStr << "] " 
                      << error.message;
            if (!error.location.empty()) {
                std::cout << " at " << error.location;
            }
            std::cout << "\n";
        }
    }
    
    size_t getErrorCount() const { return errors.size(); }
    void clear() { errors.clear(); }
};
```

### Example 2: Graceful Error Recovery

```cpp
// robust_compiler.cpp
#include "error_manager.h"
#include "ast_parser.h"
#include "semantic_analyzer.h"
#include "code_generator.h"

class RobustCompiler {
private:
    ErrorManager errorManager;
    
public:
    bool compile(const std::string& inputFile, const std::string& outputFile) {
        std::vector<ASTNode*> ast;
        
        try {
            // Phase 1: Parsing with error recovery
            ASTParser parser;
            ast = parser.parseFromFile(inputFile);
            
        } catch (const std::runtime_error& e) {
            errorManager.addError(ErrorType::PARSE_ERROR, e.what(), inputFile, 3);
            errorManager.printErrors();
            return false;
        }
        
        // Phase 2: Semantic analysis with error collection
        SymbolTable symTable;
        std::vector<std::string> semanticErrors;
        SemanticAnalyzer analyzer(symTable, semanticErrors);
        
        for (const auto* node : ast) {
            try {
                analyzer.analyze(node);
            } catch (const std::exception& e) {
                errorManager.addError(ErrorType::SEMANTIC_ERROR, e.what());
            }
        }
        
        // Collect semantic errors
        for (const auto& error : semanticErrors) {
            errorManager.addError(ErrorType::SEMANTIC_ERROR, error);
        }
        
        // Phase 3: Code generation (only if no fatal errors)
        std::ofstream outFile(outputFile);
        if (!errorManager.hasFatalErrors() && semanticErrors.empty()) {
            try {
                CodeGenerator generator(outFile);
                for (const auto* node : ast) {
                    generator.generate(node);
                }
                outFile << "Compilation successful\n";
            } catch (const std::exception& e) {
                errorManager.addError(ErrorType::CODE_GEN_ERROR, e.what(), "", 3);
            }
        } else {
            outFile << "Compilation terminated due to errors\n";
        }
        
        // Always print errors and cleanup
        if (errorManager.hasErrors()) {
            errorManager.printErrors();
        }
        
        ASTParser::cleanupAST(ast);
        return !errorManager.hasErrors();
    }
};
```

---

## Custom Extensions

### Extension 1: Adding Support for Functions

**Extended AST Node Types:**
```cpp
// In parser, recognize new node types:
// FuncDecl type name param1 param2 ...
// FuncCall name arg1 arg2 ...
// Return expr

// Example input:
Program
  FuncDecl int add x y
    BinOp +
      Id x
      Id y
  Decl int result
  Assign result
    FuncCall add
      Literal 5
      Literal 3
```

**Extended Semantic Analyzer:**
```cpp
class ExtendedSemanticAnalyzer : public SemanticAnalyzer {
private:
    struct FunctionInfo {
        std::string returnType;
        std::vector<std::string> paramTypes;
        std::vector<std::string> paramNames;
    };
    
    std::map<std::string, FunctionInfo> functions;
    
public:
    void analyze(const ASTNode* node) override {
        if (node->type == "FuncDecl") {
            analyzeFunctionDeclaration(node);
        } else if (node->type == "FuncCall") {
            analyzeFunctionCall(node);
        } else {
            SemanticAnalyzer::analyze(node);
        }
    }
    
private:
    void analyzeFunctionDeclaration(const ASTNode* node) {
        // Extract function signature and validate
        // Add to function table
    }
    
    void analyzeFunctionCall(const ASTNode* node) {
        // Validate function exists
        // Check argument types match parameters
        // Infer return type
    }
};
```

### Extension 2: Adding Array Support

**Array Declaration and Access:**
```cpp
// New node types:
// ArrayDecl type name size
// ArrayAccess name index
// Example:
Program
  ArrayDecl int arr 10
  Assign 
    ArrayAccess arr
      Literal 0
    Literal 42
```

---

## Performance Optimization

### Optimization 1: Efficient Symbol Table

```cpp
// optimized_symbol_table.h
#pragma once
#include <unordered_map>
#include <string>

class OptimizedSymbolTable {
private:
    std::unordered_map<std::string, SymbolEntry> entries;
    
public:
    void addSymbol(const std::string& name, const std::string& type) {
        if (entries.find(name) != entries.end()) {
            throw std::runtime_error("重复声明变量: " + name);
        }
        entries[name] = {name, type, false};
    }
    
    SymbolEntry* getSymbol(const std::string& name) {
        auto it = entries.find(name);
        return (it != entries.end()) ? &it->second : nullptr;
    }
    
    void setValued(const std::string& name) {
        auto it = entries.find(name);
        if (it != entries.end()) {
            it->second.isValued = true;
        }
    }
    
    size_t size() const { return entries.size(); }
    void clear() { entries.clear(); }
};
```

### Optimization 2: Memory Pool for AST Nodes

```cpp
// ast_memory_pool.h
#pragma once
#include "ast_parser.h"
#include <memory>
#include <vector>

class ASTMemoryPool {
private:
    static const size_t POOL_SIZE = 1024;
    std::vector<std::unique_ptr<ASTNode[]>> pools;
    size_t currentPool = 0;
    size_t currentIndex = 0;
    
public:
    ASTNode* allocate() {
        if (currentIndex >= POOL_SIZE) {
            pools.push_back(std::make_unique<ASTNode[]>(POOL_SIZE));
            currentPool++;
            currentIndex = 0;
        }
        
        return &pools[currentPool][currentIndex++];
    }
    
    void reset() {
        pools.clear();
        currentPool = 0;
        currentIndex = 0;
    }
};

// Usage in parser:
class PooledASTParser : public ASTParser {
private:
    ASTMemoryPool pool;
    
public:
    std::vector<ASTNode*> parseFromFile(const std::string& filename) override {
        pool.reset();
        // Use pool.allocate() instead of new ASTNode
        // No need for explicit cleanup
    }
};
```

---

## Testing Patterns

### Unit Testing Example

```cpp
// test_symbol_table.cpp
#include "gtest/gtest.h"
#include "symbol_table.h"

class SymbolTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        symTable = std::make_unique<SymbolTable>();
    }
    
    std::unique_ptr<SymbolTable> symTable;
};

TEST_F(SymbolTableTest, AddSymbolSuccess) {
    EXPECT_NO_THROW(symTable->addSymbol("x", "int"));
    
    SymbolEntry* entry = symTable->getSymbol("x");
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->name, "x");
    EXPECT_EQ(entry->type, "int");
    EXPECT_FALSE(entry->isValued);
}

TEST_F(SymbolTableTest, DuplicateSymbolThrows) {
    symTable->addSymbol("x", "int");
    EXPECT_THROW(symTable->addSymbol("x", "bool"), std::runtime_error);
}

TEST_F(SymbolTableTest, SetValuedWorks) {
    symTable->addSymbol("x", "int");
    symTable->setValued("x");
    
    SymbolEntry* entry = symTable->getSymbol("x");
    ASSERT_NE(entry, nullptr);
    EXPECT_TRUE(entry->isValued);
}
```

### Integration Testing Example

```cpp
// test_integration.cpp
#include "gtest/gtest.h"
#include "compiler_driver.h"
#include <fstream>
#include <filesystem>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::create_directories("test_input");
        std::filesystem::create_directories("test_output");
    }
    
    void TearDown() override {
        std::filesystem::remove_all("test_input");
        std::filesystem::remove_all("test_output");
    }
    
    void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file("test_input/" + filename);
        file << content;
    }
    
    std::string readOutputFile(const std::string& filename) {
        std::ifstream file("test_output/" + filename);
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
};

TEST_F(IntegrationTest, SimpleCompilation) {
    createTestFile("simple.txt", 
        "Program\n"
        "  Decl int x\n"
        "  Assign x\n"
        "    Literal 42\n");
    
    CompilerDriver compiler("test_input/simple.txt", "test_output/simple_out.txt");
    EXPECT_TRUE(compiler.compile());
    
    std::string output = readOutputFile("simple_out.txt");
    EXPECT_TRUE(output.find("MOV #42, R0") != std::string::npos);
    EXPECT_TRUE(output.find("STORE R0, x") != std::string::npos);
    EXPECT_TRUE(output.find("Compilation successful") != std::string::npos);
}
```

---

*This document provides comprehensive usage examples for the LittleC Compiler. For API reference, see API_DOCUMENTATION.md and FUNCTION_REFERENCE.md.*