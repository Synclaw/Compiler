# LittleC Compiler - Complete Documentation Index

## Welcome to the LittleC Compiler Documentation

This directory contains comprehensive documentation for the LittleC Compiler, a semantic analyzer and code generator for a simplified C-like language. The documentation is organized into several focused documents to help you understand, use, and extend the compiler.

---

## Documentation Structure

### 📖 [API_DOCUMENTATION.md](./API_DOCUMENTATION.md)
**Comprehensive API Reference and Architecture Guide**

The main documentation resource covering:
- Complete architecture overview
- Detailed API reference for all public classes and methods
- Core data structures (`ASTNode`, `SymbolEntry`)
- Integration guidelines and best practices
- Error handling patterns
- Build instructions and project setup

**Best for:** Understanding the overall system architecture and getting detailed information about each API component.

### 🔍 [FUNCTION_REFERENCE.md](./FUNCTION_REFERENCE.md)
**Quick Function Lookup Reference**

A concise reference guide providing:
- Quick lookup index for all public functions
- Function signatures and parameter details
- Return values and exception information
- Brief usage examples for each function
- Error message reference table
- Performance considerations

**Best for:** Quick lookups while coding, understanding specific function behavior, and debugging.

### 💡 [USAGE_EXAMPLES.md](./USAGE_EXAMPLES.md)
**Comprehensive Examples and Tutorials**

Practical guides including:
- Complete working code examples
- Step-by-step tutorials for common tasks
- Advanced integration patterns
- Custom extension examples
- Performance optimization techniques
- Testing patterns and best practices

**Best for:** Learning how to use the compiler, implementing common patterns, and extending functionality.

---

## Quick Start Guide

### 1. First Time Users
Start with **[API_DOCUMENTATION.md](./API_DOCUMENTATION.md)** to understand the overall architecture and then try the basic examples in **[USAGE_EXAMPLES.md](./USAGE_EXAMPLES.md)**.

### 2. Integration Developers
Focus on the "Integration Guide" section in **[API_DOCUMENTATION.md](./API_DOCUMENTATION.md)** and the "Advanced Integration Patterns" in **[USAGE_EXAMPLES.md](./USAGE_EXAMPLES.md)**.

### 3. Function Reference Lookup
Use **[FUNCTION_REFERENCE.md](./FUNCTION_REFERENCE.md)** for quick API lookups while coding.

### 4. Extending the Compiler
See "Custom Extensions" in **[USAGE_EXAMPLES.md](./USAGE_EXAMPLES.md)** and "Advanced Usage" in **[API_DOCUMENTATION.md](./API_DOCUMENTATION.md)**.

---

## Core Components Overview

The LittleC Compiler consists of four main components:

### 🔄 ASTParser
- Parses indented text format into Abstract Syntax Trees
- Handles memory management for AST nodes
- **Key Functions:** `parseFromFile()`, `cleanupAST()`

### 📊 SymbolTable
- Manages variable declarations and scope information
- Tracks variable types and initialization status
- **Key Functions:** `addSymbol()`, `getSymbol()`, `setValued()`

### ✅ SemanticAnalyzer
- Performs type checking and semantic validation
- Validates variable usage and control flow
- **Key Functions:** `analyze()`, constructor with error collection

### ⚙️ CodeGenerator
- Generates assembly-like intermediate code
- Handles control flow and expression evaluation
- **Key Functions:** `generate()`, constructor with output stream

---

## Compilation Pipeline

```
Input AST File → ASTParser → SemanticAnalyzer → CodeGenerator → Assembly Output
     ↓              ↓              ↓               ↓              ↓
parse_out.txt → AST Nodes → Symbol Table → Validated AST → Assembly Code
```

---

## Common Use Cases

### Basic Compilation
```cpp
// Complete compilation pipeline
ASTParser parser;
auto ast = parser.parseFromFile("input.txt");

SymbolTable symTable;
std::vector<std::string> errors;
SemanticAnalyzer analyzer(symTable, errors);

for (const auto* node : ast) {
    analyzer.analyze(node);
}

if (errors.empty()) {
    std::ofstream outFile("output.txt");
    CodeGenerator generator(outFile);
    for (const auto* node : ast) {
        generator.generate(node);
    }
}

ASTParser::cleanupAST(ast);
```

### Error Handling
```cpp
try {
    // Parse and analyze
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}

// Check semantic errors
if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cerr << "Semantic error: " << error << std::endl;
    }
}
```

---

## Supported Language Features

### Data Types
- `int` - Integer values
- `bool` - Boolean values

### Statements
- Variable declarations: `Decl int x`
- Assignments: `Assign x`
- Conditional statements: `IfStmt`
- Loop statements: `WhileStmt`

### Expressions
- Binary operations: `+`, `-`, `*`, `/`, `==`, `!=`, `&&`
- Variable references: `Id`
- Literal values: `Literal`

### Input Format
The compiler expects AST input in indented text format:
```
Program
  Decl int x
  Assign x
    Literal 42
  IfStmt
    BinOp ==
      Id x
      Literal 42
    Assign x
      Literal 100
```

---

## Error Types and Handling

### Parse Errors
- **Type:** `std::runtime_error` exceptions
- **Causes:** File not found, invalid format
- **Handling:** Catch exceptions during `parseFromFile()`

### Semantic Errors
- **Type:** String messages collected in vector
- **Causes:** Type mismatches, undeclared variables, uninitialized usage
- **Handling:** Check error vector after analysis

### Common Error Messages
- `"重复声明变量: [name]"` - Duplicate variable declaration
- `"未声明的变量: [name]"` - Undeclared variable usage
- `"变量未初始化: [name]"` - Uninitialized variable usage
- `"类型不匹配: [details]"` - Type mismatch in operations

---

## Build Instructions

### Prerequisites
- C++17 compatible compiler (g++ 7+)
- Make utility

### Building
```bash
make all        # Build complete project
make clean      # Remove build artifacts
```

### Project Structure
```
littlec_compiler/
├── include/           # Header files (.h)
├── src/              # Source files (.cpp)
├── input/            # Sample input files
├── output/           # Generated output files
├── Makefile          # Build configuration
├── README.md         # Project overview
└── [documentation]   # This documentation set
```

---

## Performance Considerations

### Memory Management
- AST nodes require explicit cleanup with `cleanupAST()`
- Symbol table uses linear search (O(n) lookup)
- Consider optimized versions for large programs

### Optimization Opportunities
- Use `std::unordered_map` for large symbol tables
- Implement memory pools for frequent AST operations
- Add constant folding in expression processing

---

## Extension Points

### Adding New Node Types
1. Update `ASTParser` to recognize new syntax
2. Add semantic rules in `SemanticAnalyzer::analyze()`
3. Implement code generation in `CodeGenerator::generate()`

### Custom Type Systems
1. Extend `SymbolEntry` for complex types
2. Update type checking in `processExpression()`
3. Add corresponding code generation patterns

### Advanced Features
- Function declarations and calls
- Array types and indexing
- Nested scoping
- Advanced control flow (for loops, switch statements)

---

## Getting Help

### Documentation Navigation
- **Quick API lookup:** [FUNCTION_REFERENCE.md](./FUNCTION_REFERENCE.md)
- **Detailed explanations:** [API_DOCUMENTATION.md](./API_DOCUMENTATION.md)
- **Working examples:** [USAGE_EXAMPLES.md](./USAGE_EXAMPLES.md)

### Common Workflows
1. **First-time setup:** Read API_DOCUMENTATION.md overview
2. **Basic usage:** Follow examples in USAGE_EXAMPLES.md
3. **Function details:** Look up in FUNCTION_REFERENCE.md
4. **Advanced features:** Study integration patterns in USAGE_EXAMPLES.md
5. **Troubleshooting:** Check error handling sections in all documents

---

## Contributing and Extending

This documentation covers the current state of the LittleC Compiler. When extending the compiler:

1. **Update documentation** to reflect new functionality
2. **Add examples** for new features in USAGE_EXAMPLES.md
3. **Update function reference** for new public APIs
4. **Test thoroughly** using patterns from USAGE_EXAMPLES.md

---

*This documentation was automatically generated and provides comprehensive coverage of all public APIs, functions, and components in the LittleC Compiler. Last updated: [Current Date]*