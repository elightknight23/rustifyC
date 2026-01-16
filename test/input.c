#include <stdio.h>

void analyze_me(int dynamic_index) {
    // 'volatile' forces the compiler to keep this array in the IR
    volatile int buffer[10];

    // Case 1: Constant Access
    buffer[15] = 42;

    // Case 2: Variable Access
    buffer[dynamic_index] = 99;
}

int main() {
    analyze_me(2);
    return 0;
}