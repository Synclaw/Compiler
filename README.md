# LittleC 语义分析器实现报告

---

## 一、实现思路
1. **整体架构**  
```语法树解析 → 符号表构建 → 语义分析 → 代码生成```
2. **核心技术**  
- 语法制导翻译(SDT)
- 继承属性(类型传递)
- 符号表管理(上下文信息)
- 两遍遍历策略(先语义检查后代码生成)

---

## 二、各模块函数作用

### 1. 符号表模块 (`symbol_table.h/cpp`)
| 函数/方法              | 作用描述                           |
|-----------------------|----------------------------------|
| `addSymbol()`         | 添加变量到符号表，检查重复声明       |
| `getSymbol()`         | 查询变量是否存在并返回条目指针       |
| `setValued()`         | 标记变量已赋值状态                 |

### 2. AST解析模块 (`ast_parser.h/cpp`)
| 函数                  | 核心功能                         |
|-----------------------|--------------------------------|
| `parseFromFile()`     | 解析文本构建AST，识别缩进层级      |
| `cleanupAST()`        | 递归释放AST内存                  |

### 3. 语义分析模块 (`semantic_analyzer.h/cpp`)
| 方法                  | 关键功能                       |
|-----------------------|------------------------------|
| `processExpression()` | 推导表达式类型，检查运算合法性    |
| `analyze()`           | 遍历AST执行声明/赋值/控制流检查  |

### 4. 代码生成模块 (`code_generator.h/cpp`)
| 方法                  | 功能描述                     |
|-----------------------|----------------------------|
| `newLabel()`          | 生成唯一跳转标签             |
| `generateExpression()`| 生成表达式运算指令           |
| `generate()`          | 生成赋值/条件/循环语句代码    |

---

## 三、关键核心代码

### 1. 符号表管理
```cpp
// 添加符号时检查重复声明
void SymbolTable::addSymbol(const string& name, const string& type) {
 for (const auto& entry : entries) {
     if (entry.name == name) {
         throw runtime_error("重复声明变量: " + name);
     }
 }
 
 entries.push_back({name, type, false});
}
```
### 2. 语义类型检查
```cpp
// 处理二元运算符类型校验
string SemanticAnalyzer::processExpression(const ASTNode* node) {
    if (node->type == "BinOp") {
        string ltype = processExpression(node->children[0]);
        string rtype = processExpression(node->children[1]);
        
        if (op == "+" && (ltype != "int" || rtype != "int")) {
            errors.push_back("算术运算类型错误");
            return "error";
        }
    }
}
```
### 3. 代码生成控制流
```cpp
// 生成while循环代码
void CodeGenerator::generate(const ASTNode* node) {
    if (node->type == "WhileStmt") {
        string loopStart = newLabel();
        string loopEnd = newLabel();
        
        out << loopStart << ":\n";
        generateExpression(node->children[0]); // 条件判断
        out << "JZ " << loopEnd << "\n";
        generate(node->children[1]); // 循环体
        out << "JMP " << loopStart << "\n";
        out << loopEnd << ":\n";
    }
}
```
---
## 四、待解决问题
### 1. 语义分析方面
* 数组/结构体类型：当前仅支持基础类型(int/bool)

* 函数作用域：未实现嵌套作用域管理

* 错误恢复：发现错误后立即终止，缺少错误恢复机制

### 2. 代码生成方面
* 寄存器分配：目前固定使用R0/R1，需要优化分配策略

* 浮点运算：不支持浮点数类型处理

* I/O指令：未实现read/write语句的汇编转换

### 3. 工程优化方面
* 调试信息：生成代码缺乏行号映射

* 性能分析：未添加代码执行统计功能

* 跨平台支持：汇编指令集依赖特定架构