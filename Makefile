# Makefile for littlec_compiler

# 编译器和编译选项
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude

# 项目结构
SRC_DIR := src
INC_DIR := include
OUT_DIR := output
OBJ_DIR := build

# 源文件列表
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# 对应的目标文件列表（放在 build/ 下）
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# 最终可执行文件
TARGET := littlec_compiler

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译每个源文件为目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 确保 build 和 output 目录存在
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# 清理目标
clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)/littlec_compiler

# 伪目标
.PHONY: all clean
