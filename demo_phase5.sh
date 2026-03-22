#!/bin/bash
# demo_phase5.sh - Live demonstration script for RustifyC Phase 5
# Watch how rustifyC mathematically instruments arithmetic logic!

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
echo -e "${GREEN}   RustifyC Phase 5: Integer Overflow Detection               ${NC}"
echo -e "${CYAN}================================================================${NC}"
echo ""
echo -e "${YELLOW}Goal:${NC} C famously allows Integers to overflow and wrap around."
echo "An integer wrapping from 2147483647 (INT_MAX) back to -2147483648"
echo "causes massive application logic bugs and has historically led to"
echo "millions of dollars in cryptocurrency hacks and security breaches."
echo ""
echo "RustifyC utilizes LLVM's internal '.with.overflow' intrinsics to dynamically"
echo "check the CPU flags (like the x86/ARM Overflow Flag) directly at the"
echo "hardware level after an Add/Sub/Mul operation, and conditionally"
echo "panics if an overflow occurred."
echo ""
read -p "Press [Enter] to see the code..."

clear
echo -e "${BLUE}[test_overflow_demo.c]${NC}"
cat << 'EOF' > /tmp/rustifyc_demo/test_overflow_demo.c
#include <stdio.h>
#include <limits.h>

int safe_multiply(int a, int b) {
    return a * b; // Safe
}

int unsafe_add(int a, int b) {
    return a + b; // OVERFLOWS!
}

int main(int argc, char **argv) {
    // using argc prevents the compiler from optimizing the math 
    // down to a constant string before the program runs
    int safe_a = 5 * argc;
    int safe_b = 4 * argc;

    printf("Safe Result: %d\n", safe_multiply(safe_a, safe_b));

    printf("Attempting INT_MAX + 1...\n");
    // Standard C would wrap to negative two trillion silently.
    // rustifyC will panic and abort execution.
    int overflowed = unsafe_add(INT_MAX, argc);
    printf("You will never see this line: %d\n", overflowed);
    return 0;
}
EOF

cat /tmp/rustifyc_demo/test_overflow_demo.c | awk '{print "  " $0}'
echo ""
read -p "Press [Enter] to compile with RustifyC and execute..."

clear
echo -e "${CYAN}Compiling and Executing: ./test_overflow_demo${NC}"
clang -O1 -fpass-plugin=$PASS /tmp/rustifyc_demo/test_overflow_demo.c -o /tmp/rustifyc_demo/test_overflow_demo -mllvm -stats 2>&1

echo "--------------------------------------------------------------------------------"
/tmp/rustifyc_demo/test_overflow_demo
EXIT_CODE=$?
echo "--------------------------------------------------------------------------------"

if [ $EXIT_CODE -eq 101 ]; then
    echo -e "${GREEN}SUCCESS: We caught the hardware overflow flag and aborted with code 101!${NC}"
else
    echo -e "${RED}FAILURE: Binary wrapped around silently!${NC}"
fi

echo ""
rm -rf /tmp/rustifyc_demo
echo -e "${CYAN}Demo complete.${NC}"
