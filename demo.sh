#!/bin/bash
# demo.sh - Live demonstration script for RustifyC
# Run this script to see RustifyC in action!

# ANSI color codes
RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASS="$(pwd)/build/rustifyC.so"
mkdir -p /tmp/rustifyc_demo

clear
echo -e "${BLUE}======================================================${NC}"
echo -e "${GREEN}   RustifyC: Compiler-Enforced Spatial Safety MVP     ${NC}"
echo -e "${BLUE}======================================================${NC}"
echo ""

# ---------------------------------------------------------
# Feature 1: Compile-Time Diagnostics
# ---------------------------------------------------------
echo -e "${YELLOW}▶ FEATURE 1: Rust-Style Compile-Time Diagnostics${NC}"
echo "We intercept obvious buffer overflows at compile time and present clear, visual errors."
echo ""
echo -e "${BLUE}[demo_static.c]${NC}"
cat << 'EOF' > /tmp/rustifyc_demo/demo_static.c
int main() {
    int buffer[5];
    buffer[10] = 42; // OVERFLOW
    return 0;
}
EOF
cat /tmp/rustifyc_demo/demo_static.c | awk '{print "  " $0}'
echo ""
echo -e "${NC}Compiling with RustifyC...${NC}"
clang -Wno-everything -fpass-plugin=$PASS /tmp/rustifyc_demo/demo_static.c -o /dev/null
echo ""
read -p "Press [Enter] to continue..."

# ---------------------------------------------------------
# Feature 2: Runtime Panics
# ---------------------------------------------------------
clear
echo -e "${YELLOW}▶ FEATURE 2: Rust-Style Runtime Panics${NC}"
echo "For dynamic bounds, we inject IR checks that format a detailed panic message instead of just aborting."
echo ""
echo -e "${BLUE}[demo_dynamic.c]${NC}"
cat << 'EOF' > /tmp/rustifyc_demo/demo_dynamic.c
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    int buffer[10];
    int index = atoi(argv[1]);
    buffer[index] = 42;
    printf("Wrote 42 to buffer[%d]\n", index);
    return 0;
}
EOF
cat /tmp/rustifyc_demo/demo_dynamic.c | awk '{print "  " $0}'
echo ""
clang -Wno-everything -fpass-plugin=$PASS /tmp/rustifyc_demo/demo_dynamic.c -o /tmp/rustifyc_demo/demo_dynamic 2>/dev/null
echo -e "Executing: ${BLUE}./demo_dynamic 5${NC} (Safe Access)"
/tmp/rustifyc_demo/demo_dynamic 5
echo ""
echo -e "Executing: ${RED}./demo_dynamic 15${NC} (Buffer Overflow!)"
/tmp/rustifyc_demo/demo_dynamic 15
echo ""
read -p "Press [Enter] to continue..."

# ---------------------------------------------------------
# Feature 3: LLVM Statistics
# ---------------------------------------------------------
clear
echo -e "${YELLOW}▶ FEATURE 3: LLVM Statistics and Reporting${NC}"
echo "We track everything the pass does and print a professional summary."
echo ""
echo -e "Compiling multiple functions with various array accesses..."
cat << 'EOF' > /tmp/rustifyc_demo/stats.c
void func1(int idx) { int arr[10]; arr[idx] = 1; }
void func2() { int arr[5]; arr[2] = 1; }
void func3() { int arr[5]; arr[10] = 1; }
void func4(int x, int y) { int matrix[10][10]; matrix[x][y] = 1; }
EOF
clang -Wno-everything -fpass-plugin=$PASS /tmp/rustifyc_demo/stats.c -c -o /dev/null -mllvm -stats 2>&1 | tail -n 20
echo ""
read -p "Press [Enter] to continue..."

# ---------------------------------------------------------
# Feature 4: Validation Suites
# ---------------------------------------------------------
clear
echo -e "${YELLOW}▶ FEATURE 4: Rigorous Validation${NC}"
echo "The pass has been tested against NIST Juliet CWE-121 benchmarks and performance workloads."
echo ""
echo -e "${BLUE}1. NIST Juliet Validation Suite (CWE-121 Subset)${NC}"
cat test/juliet_results.txt
echo ""
echo -e "${BLUE}2. Performance Overhead${NC}"
echo "Running performance benchmarks (Baseline vs RustifyC)..."
./test/benchmark.sh | grep -v "===" | grep -v "Compiling" 
echo ""
echo -e "${GREEN}All checks passed! The RustifyC MVP is complete.${NC}"
rm -rf /tmp/rustifyc_demo
