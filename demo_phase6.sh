#!/bin/bash
set -e

# ANSI color codes for consistency with demo.sh
RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
YELLOW='\033[1;33m'
CYAN='\033[1;36m'
NC='\033[0m'

echo -e "${CYAN}================================================================${NC}"
echo -e "${GREEN}  RustifyC Phase 6: Stack Escape Detection (Temporal Safety)${NC}"
echo -e "${CYAN}================================================================${NC}"
echo ""

PLUGIN="$(pwd)/build/rustifyC.so"

# Test 1: Return escape
echo -e "${YELLOW}Test 1: Returning stack address${NC}"
echo "--------------------------------"
cat test/test_stack_escape_return.c
echo ""
echo -e "${BLUE}Compiling with rustifyC...${NC}"
clang -g -fpass-plugin=$PLUGIN test/test_stack_escape_return.c -o test1 2>&1 || true
echo ""

# Test 2: Global escape
echo -e "${YELLOW}Test 2: Storing to global variable${NC}"
echo "-----------------------------------"
cat test/test_stack_escape_global.c
echo ""
echo -e "${BLUE}Compiling with rustifyC...${NC}"
clang -g -fpass-plugin=$PLUGIN test/test_stack_escape_global.c -o test2 2>&1 || true
echo ""

# Test 3: Safe cases
echo -e "${YELLOW}Test 3: Safe stack usage (should compile)${NC}"
echo "------------------------------------------"
cat test/test_stack_safe.c
echo ""
echo -e "${BLUE}Compiling with rustifyC...${NC}"
clang -g -fpass-plugin=$PLUGIN test/test_stack_safe.c -o test3 && echo -e "${GREEN}✓ Compiled successfully!${NC}"
echo ""

echo -e "${CYAN}================================================================${NC}"
echo -e "${GREEN}  Phase 6 Demo Complete${NC}"
echo -e "${CYAN}================================================================${NC}"
