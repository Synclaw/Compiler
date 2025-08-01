# LittleC Compiler - Function Reference Guide

## Quick Reference Index

### ASTParser Class
- [`parseFromFile()`](#astparserparseFromFile) - Parse AST from text file
- [`cleanupAST()`](#astparsercleanupast) - Clean up AST memory

### SymbolTable Class  
- [`addSymbol()`](#symboltableaddsymbol) - Add variable to symbol table
- [`getSymbol()`](#symboltablegetsymbol) - Lookup variable by name
- [`setValued()`](#symboltablesetvalued) - Mark variable as initialized

### SemanticAnalyzer Class
- [`SemanticAnalyzer()`](#semanticanalyzerconstructor) - Constructor
- [`analyze()`](#semanticanalyzeranalyze) - Perform semantic analysis

### CodeGenerator Class
- [`CodeGenerator()`](#codegeneratorconstructor) - Constructor  
- [`generate()`](#codegeneratorgenerate) - Generate assembly code

### Data Structures
- [`ASTNode`](#astnode) - AST node structure
- [`SymbolEntry`](#symbolentry) - Symbol table entry

---

## Detailed Function Reference

### ASTParser::parseFromFile()

```cpp
std::vector<ASTNode*> parseFromFile(const std::string& filename)
```

**Purpose:** Parses an Abstract Syntax Tree from a text file with indented format.

**Parameters:**
- `filename` (const std::string&) - Path to input file containing AST representation

**Return Value:**
- `std::vector<ASTNode*>` - Vector containing root nodes of parsed AST

**Exceptions:**
- `std::runtime_error` - Thrown when file cannot be opened

**Input Format:**
The expected input format uses indentation (2 spaces per level) to represent tree structure:
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
    auto ast = parser.parseFromFile("input/program.txt");
    std::cout << "Parsed " << ast.size() << " root nodes" << std::endl;
    
    // Process AST...
    
    ASTParser::cleanupAST(ast);
} catch (const std::runtime_error& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

**Node Type Recognition:**
- `Decl` - Variable declaration with type and identifier
- `Assign` - Assignment statement with target identifier  
- `BinOp` - Binary operation with operator
- Other node types are parsed generically

---

### ASTParser::cleanupAST()

```cpp
static void cleanupAST(std::vector<ASTNode*>& nodes)
```

**Purpose:** Recursively deallocates memory for all AST nodes in the tree.

**Parameters:**
- `nodes` (std::vector<ASTNode*>&) - Reference to vector of AST nodes to deallocate

**Return Value:** void

**Usage Example:**
```cpp
std::vector<ASTNode*> ast = parser.parseFromFile("input.txt");

// Use AST for processing...

// Clean up when done
ASTParser::cleanupAST(ast);
// ast vector is now empty and all nodes are deallocated
```

**Implementation Notes:**
- Performs depth-first recursive cleanup of all child nodes
- Clears the input vector after deallocation
- Safe to call on empty vectors

---

### SymbolTable::addSymbol()

```cpp
void addSymbol(const std::string& name, const std::string& type)
```

**Purpose:** Adds a new variable symbol to the symbol table.

**Parameters:**
- `name` (const std::string&) - Variable name/identifier
- `type` (const std::string&) - Variable type (e.g., "int", "bool")

**Return Value:** void

**Exceptions:**
- `std::runtime_error` - Thrown if variable name already exists in symbol table

**Usage Example:**
```cpp
SymbolTable symTable;

try {
    symTable.addSymbol("x", "int");
    symTable.addSymbol("flag", "bool");
    symTable.addSymbol("count", "int");
} catch (const std::runtime_error& e) {
    std::cerr << "Symbol error: " << e.what() << std::endl;
    // Handle duplicate declaration
}
```

**Validation Rules:**
- Variable names must be unique within the symbol table
- No validation is performed on type names
- New symbols are initially marked as not valued (`isValued = false`)

---

### SymbolTable::getSymbol()

```cpp
SymbolEntry* getSymbol(const std::string& name)
```

**Purpose:** Retrieves a symbol entry by variable name.

**Parameters:**
- `name` (const std::string&) - Variable name to lookup

**Return Value:**
- `SymbolEntry*` - Pointer to symbol entry if found, `nullptr` if not found

**Usage Example:**
```cpp
SymbolEntry* entry = symTable.getSymbol("x");
if (entry) {
    std::cout << "Found variable: " << entry->name 
              << " (type: " << entry->type 
              << ", valued: " << entry->isValued << ")" << std::endl;
} else {
    std::cout << "Variable 'x' not found" << std::endl;
}
```

**Return Value Details:**
- Non-null pointer: Variable exists, can access all SymbolEntry fields
- Null pointer: Variable not declared in symbol table

---

### SymbolTable::setValued()

```cpp
void setValued(const std::string& name)
```

**Purpose:** Marks a variable as having been assigned a value.

**Parameters:**
- `name` (const std::string&) - Variable name to mark as valued

**Return Value:** void

**Usage Example:**
```cpp
// After variable declaration
symTable.addSymbol("x", "int");

// After assignment processing
symTable.setValued("x");

// Check if valued
SymbolEntry* entry = symTable.getSymbol("x");
if (entry && entry->isValued) {
    std::cout << "Variable x has been initialized" << std::endl;
}
```

**Behavior:**
- If variable exists: Sets `isValued = true`
- If variable doesn't exist: No operation (silent failure)
- Used during semantic analysis to track variable initialization

---

### SemanticAnalyzer Constructor

```cpp
SemanticAnalyzer(SymbolTable& st, std::vector<std::string>& errs)
```

**Purpose:** Constructs a semantic analyzer with references to symbol table and error collection.

**Parameters:**
- `st` (SymbolTable&) - Reference to symbol table for variable management
- `errs` (std::vector<std::string>&) - Reference to error vector for collecting semantic errors

**Usage Example:**
```cpp
SymbolTable symTable;
std::vector<std::string> errors;
SemanticAnalyzer analyzer(symTable, errors);

// Use analyzer...
analyzer.analyze(astNode);

// Check for errors
if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cerr << "Semantic error: " << error << std::endl;
    }
}
```

**Design Notes:**
- Uses references to avoid copying large objects
- Errors are accumulated rather than thrown for better error reporting
- Symbol table is shared with code generation phase

---

### SemanticAnalyzer::analyze()

```cpp
void analyze(const ASTNode* node)
```

**Purpose:** Performs comprehensive semantic analysis on an AST node and its children.

**Parameters:**
- `node` (const ASTNode*) - Root node of AST subtree to analyze

**Return Value:** void

**Analysis Performed:**

1. **Variable Declarations (`Decl`):**
   - Adds variables to symbol table
   - Detects duplicate declarations

2. **Assignment Statements (`Assign`):**
   - Validates variable is declared
   - Checks type compatibility
   - Marks variables as valued

3. **Control Flow (`IfStmt`, `WhileStmt`):**
   - Validates condition expressions are boolean
   - Recursively analyzes child statements

4. **Expression Validation:**
   - Type checking for binary operations
   - Variable existence and initialization checking
   - Type inference for complex expressions

**Usage Example:**
```cpp
std::vector<ASTNode*> ast = parser.parseFromFile("input.txt");
SymbolTable symTable;
std::vector<std::string> errors;
SemanticAnalyzer analyzer(symTable, errors);

for (const auto* node : ast) {
    analyzer.analyze(node);
}

if (errors.empty()) {
    std::cout << "Semantic analysis passed" << std::endl;
} else {
    std::cout << "Found " << errors.size() << " semantic errors" << std::endl;
}
```

**Error Detection:**
- Undeclared variables
- Type mismatches
- Uninitialized variable usage
- Invalid operations for types

---

### CodeGenerator Constructor

```cpp
CodeGenerator(std::ofstream& o)
```

**Purpose:** Constructs a code generator with output stream for assembly code.

**Parameters:**
- `o` (std::ofstream&) - Reference to output file stream

**Usage Example:**
```cpp
std::ofstream outFile("output/assembly.txt");
CodeGenerator generator(outFile);

// Generate code for AST nodes
for (const auto* node : ast) {
    generator.generate(node);
}

outFile.close();
```

---

### CodeGenerator::generate()

```cpp
void generate(const ASTNode* node)
```

**Purpose:** Generates assembly-like intermediate code for an AST node and its children.

**Parameters:**
- `node` (const ASTNode*) - AST node to generate code for

**Return Value:** void

**Supported Node Types:**

1. **Program**: Recursively generates code for all children
2. **Assign**: Generates assignment with expression evaluation
3. **IfStmt**: Generates conditional branching with labels
4. **WhileStmt**: Generates loop with condition testing
5. **Expression nodes**: Handled by internal `generateExpression()`

**Generated Instruction Set:**
```
MOV #value, Rn    - Move immediate value to register
LOAD var, Rn      - Load variable to register  
STORE Rn, var     - Store register to variable
ADD R1, R0        - Add R1 to R0
SUB R1, R0        - Subtract R1 from R0
AND R1, R0        - Logical AND
TEST Rn           - Test register value
JMP label         - Unconditional jump
JZ label          - Jump if zero
PUSH Rn           - Push register to stack
POP Rn            - Pop stack to register
```

**Usage Example:**
```cpp
std::ofstream codeFile("output/code.asm");
CodeGenerator generator(codeFile);

// Generate for simple assignment
ASTNode* assign = /* assignment node */;
generator.generate(assign);
// Output: MOV #42, R0
//         STORE R0, x

codeFile.close();
```

**Label Generation:**
- Automatically generates unique labels (L0, L1, L2, ...)
- Used for control flow (if/else, loops)
- Label counter maintained internally

---

## Data Structures Reference

### ASTNode

```cpp
struct ASTNode {
    std::string type;                               // Node type identifier
    std::map<std::string, std::string> attributes;  // Key-value attributes  
    std::vector<ASTNode*> children;                 // Child node pointers
    
    std::string getAttribute(const std::string& key) const;
};
```

**Fields:**
- `type`: Identifies the kind of AST node ("Decl", "Assign", "BinOp", etc.)
- `attributes`: Stores node-specific data (variable names, operators, values)
- `children`: Contains child nodes for tree structure

**getAttribute() Method:**
- Returns attribute value for given key
- Returns empty string if key doesn't exist
- Const method for safe access

**Common Attribute Keys:**
- `"id"` - Variable identifier (Decl, Assign, Id nodes)
- `"type"` - Variable type (Decl nodes)  
- `"op"` - Operator (BinOp nodes)
- `"value"` - Literal value (Literal, Id nodes)

---

### SymbolEntry

```cpp
struct SymbolEntry {
    std::string name;        // Variable identifier
    std::string type;        // Variable type  
    bool isValued = false;   // Initialization status
};
```

**Fields:**
- `name`: Variable identifier as declared
- `type`: Type string ("int", "bool", custom types)
- `isValued`: Tracks whether variable has been assigned

**Usage in Symbol Table:**
- Created by `addSymbol()`
- Accessed via `getSymbol()`
- Modified by `setValued()`

---

## Error Reference

### Exception Types

| Function | Exception Type | Trigger Condition |
|----------|----------------|-------------------|
| `parseFromFile()` | `std::runtime_error` | File cannot be opened |
| `addSymbol()` | `std::runtime_error` | Duplicate variable name |

### Semantic Error Messages

| Error Type | Message Format | Trigger Condition |
|------------|----------------|-------------------|
| Undeclared Variable | `"未声明的变量: [name]"` | Using undeclared variable |
| Uninitialized Variable | `"变量未初始化: [name]"` | Using unassigned variable |
| Duplicate Declaration | `"重复声明变量: [name]"` | Declaring variable twice |
| Type Mismatch | `"类型不匹配: [details]"` | Assignment type incompatibility |
| Arithmetic Type Error | `"算术运算类型不匹配: [details]"` | Non-int arithmetic operands |
| Comparison Type Error | `"比较运算类型不兼容: [details]"` | Incompatible comparison types |
| Boolean Condition Error | `"条件表达式必须是布尔型"` | Non-bool if/while condition |

---

## Performance Notes

### Memory Management
- AST nodes allocated with `new`, must call `cleanupAST()`
- Symbol table uses vector storage, O(n) lookup
- Error collection uses vector, efficient for small error counts

### Optimization Recommendations
- Use `std::unordered_map` for large symbol tables
- Implement move semantics for AST manipulation
- Consider memory pools for frequent AST creation/destruction

---

*This reference guide provides quick lookup for all public APIs. For detailed usage patterns and integration examples, see API_DOCUMENTATION.md*