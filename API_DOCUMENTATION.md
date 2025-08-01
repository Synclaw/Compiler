# LittleC Compiler - Comprehensive API Documentation

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Core Data Structures](#core-data-structures)
4. [Public APIs](#public-apis)
   - [ASTParser](#astparser)
   - [SymbolTable](#symboltable)
   - [SemanticAnalyzer](#semanticanalyzer)
   - [CodeGenerator](#codegenerator)
5. [Usage Examples](#usage-examples)
6. [Integration Guide](#integration-guide)
7. [Error Handling](#error-handling)
8. [Build Instructions](#build-instructions)

---

## Overview

The LittleC Compiler is a semantic analyzer and code generator for a simplified C-like language. It processes Abstract Syntax Trees (ASTs), performs semantic analysis with type checking, and generates assembly-like intermediate code.

**Key Features:**
- AST parsing from text format
- Symbol table management with scope handling
- Type checking and semantic analysis
- Assembly code generation
- Comprehensive error reporting

---

## Architecture

The compiler follows a traditional multi-pass architecture:

```
Input AST → Symbol Table Construction → Semantic Analysis → Code Generation → Output
```

**Core Components:**
1. **ASTParser**: Parses indented text format into AST structures
2. **SymbolTable**: Manages variable declarations and scoping
3. **SemanticAnalyzer**: Performs type checking and semantic validation
4. **CodeGenerator**: Produces assembly-like intermediate code

---

## Core Data Structures

### ASTNode

Represents a node in the Abstract Syntax Tree.

```cpp
struct ASTNode {
    std::string type;                                    // Node type (e.g., "Decl", "Assign", "BinOp")
    std::map<std::string, std::string> attributes;       // Node attributes (e.g., variable names, operators)
    std::vector<ASTNode*> children;                      // Child nodes
    
    std::string getAttribute(const std::string& key) const;
};
```

**Supported Node Types:**
- `Program`: Root program node
- `Decl`: Variable declaration
- `Assign`: Assignment statement
- `BinOp`: Binary operation
- `IfStmt`: Conditional statement
- `WhileStmt`: Loop statement
- `Id`: Identifier reference
- `Literal`: Numeric literal

### SymbolEntry

Represents an entry in the symbol table.

```cpp
struct SymbolEntry {
    std::string name;        // Variable name
    std::string type;        // Variable type (e.g., "int", "bool")
    bool isValued = false;   // Whether variable has been assigned a value
};
```

---

## Public APIs

### ASTParser

Parses indented text format into AST structures.

#### Methods

##### `std::vector<ASTNode*> parseFromFile(const std::string& filename)`

Parses an AST from a text file with indented format.

**Parameters:**
- `filename`: Path to the input file containing the AST representation

**Returns:**
- Vector of root AST nodes

**Throws:**
- `std::runtime_error`: If file cannot be opened

**Example Input Format:**
```
Program
  Decl int x
  Assign x
    Literal 42
  BinOp +
    Id x
    Literal 10
```

**Usage Example:**
```cpp
ASTParser parser;
try {
    std::vector<ASTNode*> ast = parser.parseFromFile("input/program.txt");
    // Process AST nodes...
} catch (const std::exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

##### `static void cleanupAST(std::vector<ASTNode*>& nodes)`

Recursively deallocates memory for AST nodes.

**Parameters:**
- `nodes`: Vector of AST nodes to deallocate

**Usage Example:**
```cpp
std::vector<ASTNode*> ast = parser.parseFromFile("input.txt");
// ... use AST ...
ASTParser::cleanupAST(ast);  // Clean up memory
```

---

### SymbolTable

Manages variable declarations and scope information.

#### Methods

##### `void addSymbol(const std::string& name, const std::string& type)`

Adds a new symbol to the symbol table.

**Parameters:**
- `name`: Variable name
- `type`: Variable type (e.g., "int", "bool")

**Throws:**
- `std::runtime_error`: If variable is already declared

**Usage Example:**
```cpp
SymbolTable symTable;
try {
    symTable.addSymbol("x", "int");
    symTable.addSymbol("flag", "bool");
} catch (const std::runtime_error& e) {
    std::cerr << "Symbol table error: " << e.what() << std::endl;
}
```

##### `SymbolEntry* getSymbol(const std::string& name)`

Retrieves a symbol entry by name.

**Parameters:**
- `name`: Variable name to lookup

**Returns:**
- Pointer to SymbolEntry if found, nullptr otherwise

**Usage Example:**
```cpp
SymbolEntry* entry = symTable.getSymbol("x");
if (entry) {
    std::cout << "Variable " << entry->name << " has type " << entry->type << std::endl;
    std::cout << "Is valued: " << (entry->isValued ? "yes" : "no") << std::endl;
} else {
    std::cout << "Variable not found" << std::endl;
}
```

##### `void setValued(const std::string& name)`

Marks a variable as having been assigned a value.

**Parameters:**
- `name`: Variable name to mark as valued

**Usage Example:**
```cpp
symTable.addSymbol("x", "int");
// ... after assignment occurs ...
symTable.setValued("x");
```

---

### SemanticAnalyzer

Performs semantic analysis including type checking and variable validation.

#### Constructor

```cpp
SemanticAnalyzer(SymbolTable& st, std::vector<std::string>& errs)
```

**Parameters:**
- `st`: Reference to symbol table for variable lookup
- `errs`: Reference to error vector for collecting semantic errors

#### Methods

##### `void analyze(const ASTNode* node)`

Performs semantic analysis on an AST node and its children.

**Parameters:**
- `node`: AST node to analyze

**Analysis Performed:**
- Variable declaration validation
- Assignment type checking
- Expression type inference
- Control flow condition validation
- Variable initialization checking

**Usage Example:**
```cpp
SymbolTable symTable;
std::vector<std::string> errors;
SemanticAnalyzer analyzer(symTable, errors);

for (const auto* node : ast) {
    analyzer.analyze(node);
}

if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cerr << "Semantic error: " << error << std::endl;
    }
}
```

##### `std::string processExpression(const ASTNode* node)` (private)

Infers the type of an expression and validates type compatibility.

**Supported Operations:**
- Arithmetic: `+`, `-`, `*`, `/` (requires int operands, returns int)
- Comparison: `==`, `!=` (requires compatible types, returns bool)
- Logical: `&&`, `||` (requires bool operands, returns bool)

---

### CodeGenerator

Generates assembly-like intermediate code from validated AST.

#### Constructor

```cpp
CodeGenerator(std::ofstream& o)
```

**Parameters:**
- `o`: Output stream for generated code

#### Methods

##### `void generate(const ASTNode* node)`

Generates code for an AST node and its children.

**Parameters:**
- `node`: AST node to generate code for

**Supported Code Generation:**
- Variable assignments
- Conditional statements (if/else)
- Loop statements (while)
- Expression evaluation
- Control flow with labels and jumps

**Generated Instruction Set:**
- `MOV #value, Rn`: Move immediate value to register
- `LOAD var, Rn`: Load variable value to register
- `STORE Rn, var`: Store register value to variable
- `ADD R1, R0`: Add registers
- `SUB R1, R0`: Subtract registers
- `AND R1, R0`: Logical AND
- `TEST Rn`: Test register value
- `JMP label`: Unconditional jump
- `JZ label`: Jump if zero
- `PUSH Rn`: Push register to stack
- `POP Rn`: Pop stack to register

**Usage Example:**
```cpp
std::ofstream outFile("output/code.asm");
CodeGenerator generator(outFile);

for (const auto* node : ast) {
    generator.generate(node);
}
outFile.close();
```

##### `std::string newLabel()` (private)

Generates unique labels for control flow.

**Returns:**
- Unique label string (e.g., "L0", "L1", "L2")

---

## Usage Examples

### Complete Compilation Pipeline

```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "ast_parser.h"
#include "semantic_analyzer.h"
#include "code_generator.h"

int main() {
    // 1. Parse AST from file
    ASTParser parser;
    std::vector<ASTNode*> ast;
    try {
        ast = parser.parseFromFile("input/program.txt");
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return 1;
    }

    // 2. Semantic analysis
    SymbolTable symTable;
    std::vector<std::string> errors;
    SemanticAnalyzer analyzer(symTable, errors);
    
    for (const auto* node : ast) {
        analyzer.analyze(node);
    }

    // 3. Generate output
    std::ofstream outFile("output/result.txt");
    if (!errors.empty()) {
        for (const auto& err : errors) {
            outFile << "Semantic error: " << err << "\n";
        }
        outFile << "Compilation terminated\n";
    } else {
        CodeGenerator generator(outFile);
        for (const auto* node : ast) {
            generator.generate(node);
        }
        outFile << "Compilation successful\n";
    }

    // 4. Cleanup
    ASTParser::cleanupAST(ast);
    return 0;
}
```

### Example Input and Output

**Input AST (input/parse_out.txt):**
```
Program
  Decl int x
  Assign x
    Literal 42
  Decl int y  
  Assign y
    BinOp +
      Id x
      Literal 10
```

**Generated Assembly Output:**
```
MOV #42, R0
STORE R0, x
LOAD x, R0
PUSH R0
MOV #10, R0
POP R1
ADD R1, R0
STORE R0, y
Compilation successful
```

### Error Handling Example

**Input with Semantic Errors:**
```
Program
  Assign x        ; Error: undeclared variable
    Literal 42
  Decl int y
  Assign y        ; Error: type mismatch
    Literal true
```

**Error Output:**
```
Semantic error: 赋值给未声明变量: x
Semantic error: 类型不匹配: y 期望 int 实际 bool
Compilation terminated
```

---

## Integration Guide

### Including the Compiler in Your Project

1. **Add include directories:**
   ```cpp
   #include "ast_parser.h"
   #include "symbol_table.h"
   #include "semantic_analyzer.h"
   #include "code_generator.h"
   ```

2. **Link against compiled objects:**
   ```makefile
   OBJS = ast_parser.o symbol_table.o semantic_analyzer.o code_generator.o
   your_program: your_main.o $(OBJS)
       g++ -o $@ $^
   ```

### Custom AST Processing

To process ASTs programmatically instead of from files:

```cpp
// Create AST nodes manually
ASTNode* program = new ASTNode{"Program", {}, {}};
ASTNode* decl = new ASTNode{"Decl", {{"type", "int"}, {"id", "x"}}, {}};
ASTNode* assign = new ASTNode{"Assign", {{"id", "x"}}, {}};
ASTNode* literal = new ASTNode{"Literal", {{"value", "42"}}, {}};

// Build tree structure
program->children.push_back(decl);
program->children.push_back(assign);
assign->children.push_back(literal);

// Process with analyzer
std::vector<ASTNode*> ast = {program};
// ... semantic analysis and code generation ...

// Don't forget cleanup
ASTParser::cleanupAST(ast);
```

---

## Error Handling

### Exception Types

1. **File I/O Errors:**
   - Thrown by: `ASTParser::parseFromFile()`
   - Type: `std::runtime_error`
   - Causes: File not found, permission denied

2. **Symbol Table Errors:**
   - Thrown by: `SymbolTable::addSymbol()`
   - Type: `std::runtime_error`
   - Causes: Duplicate variable declaration

### Semantic Error Collection

Semantic errors are collected in a vector instead of throwing exceptions:

```cpp
std::vector<std::string> errors;
SemanticAnalyzer analyzer(symTable, errors);
analyzer.analyze(node);

// Check for errors
if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cerr << "Error: " << error << std::endl;
    }
    // Handle compilation failure
}
```

### Common Error Messages

- `"重复声明变量: [name]"` - Variable declared multiple times
- `"未声明的变量: [name]"` - Use of undeclared variable
- `"变量未初始化: [name]"` - Use of uninitialized variable
- `"类型不匹配: [details]"` - Type mismatch in assignment or operation
- `"算术运算类型不匹配: [details]"` - Invalid types for arithmetic operation
- `"条件表达式必须是布尔型"` - Non-boolean condition in if/while

---

## Build Instructions

### Prerequisites

- C++17 compatible compiler (g++ 7+)
- Make utility

### Building the Project

```bash
# Clone/navigate to project directory
cd littlec_compiler

# Build all components
make all

# This creates the executable: littlec_compiler
```

### Build Targets

```bash
make all        # Build complete project
make clean      # Remove build artifacts
make $(TARGET)  # Build specific target
```

### Custom Build Configuration

Modify `Makefile` for custom settings:

```makefile
# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Iinclude -O2 -DNDEBUG

# Debug build
CXXFLAGS := -std=c++17 -Wall -Iinclude -g -DDEBUG
```

### Directory Structure

```
littlec_compiler/
├── include/           # Header files (.h)
├── src/              # Source files (.cpp)  
├── input/            # Sample input files
├── output/           # Generated output files
├── build/            # Compiled object files
├── Makefile          # Build configuration
└── README.md         # Project documentation
```

---

## Advanced Usage

### Extending Node Types

To add support for new AST node types:

1. **Update parser** to recognize new syntax
2. **Add semantic rules** in `SemanticAnalyzer::analyze()`
3. **Implement code generation** in `CodeGenerator::generate()`

### Custom Type System

To extend beyond int/bool types:

1. **Modify** `SymbolEntry` structure for complex types
2. **Update** type checking rules in `processExpression()`
3. **Add** corresponding code generation patterns

### Performance Optimization

- Use `std::unordered_map` for symbol table lookup in large programs
- Implement register allocation for better code generation
- Add constant folding in expression processing

---

*This documentation covers the complete public API of the LittleC Compiler. For implementation details, refer to the source code in the `src/` directory.*