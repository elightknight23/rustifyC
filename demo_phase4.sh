#!/bin/bash
# demo_phase4.sh - Live demonstration script for RustifyC Phase 4
# Watch how rustifyC catches uninitialized memory reads at compile-time and runtime!

RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
YELLOW='\033[1;33m'
CYAN='\033[1;36m'
NC='\033[0m'

PASS="$(pwd)/build/rustifyC.so"
mkdir -p /tmp/rustifyc_demo

clear
echo -e "${CYAN}================================================================${NC}"
echo -e "${GREEN}   RustifyC Phase 4: Uninitialized Memory Detection           ${NC}"
echo -e "${CYAN}================================================================${NC}"
echo ""
echo -e "${YELLOW}Goal:${NC} Catching code that reads from a variable before ever writing to it."
echo "Native C compilers might warn you if they're smart, but they will still compile "
echo "it and let it run, causing Undefined Behavior and potentially leaking sensitive memory."
echo ""
echo "RustifyC uses LLVM's MemorySSA (Single Static Assignment) to trace Def-Use chains."
echo "If an instruction reads from an address without a preceding store operation,"
echo "we print a Rust-like 'error[E0381]' error AND inject an unconditional runtime"
echo "panic trap to prevent exploitation!"
echo ""
read -p "Press [Enter] to see the code..."

clear
echo -e "${BLUE}[test_uninit_demo.c]${NC}"
cat << 'EOF' > /tmp/rustifyc_demo/test_uninit_demo.c
#include <stdio.h>

void safe_function() {
    int x = 42;
    printf("Safe memory read: %d\n", x);
}

void buggy_function() {
    int secret_variable;
    
    // Uh oh. The programmer forgot to assign 'secret_variable'.
    // In C, this prints whatever sensitive data was left on the stack frame!
    // RustifyC will catch this statically and inject a panic block!
    printf("Uninitialized memory leak: %d\n", secret_variable);
}

int main() {
    safe_function();
    buggy_function();
    return 0;
}
EOF

cat /tmp/rustifyc_demo/test_uninit_demo.c | awk '{print "  " $0}'
echo ""
read -p "Press [Enter] to compile with RustifyC (Note we use -O0 to simulate debug builds)..."

clear
echo -e "${CYAN}Compiling with RustifyC...${NC}"
# Compile with debug info (-g) and disable aggressive optimization so we can catch local stack reads
clang -O0 -Xclang -disable-O0-optnone -g -fpass-plugin=$PASS /tmp/rustifyc_demo/test_uninit_demo.c -o /tmp/rustifyc_demo/test_uninit_demo -mllvm -stats 2>&1

echo ""
echo -e "${GREEN}Notice: rustifyC caught the definitely uninitialized read at compile-time!${NC}"
echo -e "${YELLOW}Notice: rustifyC injected an unconditional exit-trap into the binary!${NC}"
echo ""
read -p "Press [Enter] to execute the compiled binary..."

clear
echo -e "${CYAN}Executing: ./test_uninit_demo${NC}"
echo "--------------------------------------------------------------------------------"
/tmp/rustifyc_demo/test_uninit_demo
EXIT_CODE=$?
echo "--------------------------------------------------------------------------------"

if [ $EXIT_CODE -eq 101 ]; then
    echo -e "${GREEN}SUCCESS: The binary trapped and safely aborted with exit code 101 (Rust standard)!${NC}"
else
    echo -e "${RED}FAILURE: Binary didn't trap!${NC}"
fi

echo ""
rm -rf /tmp/rustifyc_demo
echo -e "${CYAN}Demo complete.${NC}"
