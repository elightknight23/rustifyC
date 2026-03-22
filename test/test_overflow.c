#include <stdio.h>
#include <limits.h>

void test_add_overflow(int a, int b) {
    // This will overflow if a + b > INT_MAX
    int result = a + b;
    printf("Add Result: %d\n", result);
}

void test_sub_overflow(int a, int b) {
    int result = a - b;
    printf("Sub Result: %d\n", result);
}

void test_mul_overflow(int a, int b) {
    int result = a * b;
    printf("Mul Result: %d\n", result);
}

int main(int argc, char **argv) {
    // Use argc to prevent Clang from optimizing away the arithmetic
    int safe_a = 10 * argc;
    int safe_b = 20 * (argc == 1 ? 1 : 0); // Still 20 if running without args
    
    printf("Testing safe operations...\n");
    test_add_overflow(safe_a, safe_b);
    test_sub_overflow(50 * argc, 10);
    test_mul_overflow(5 * argc, 4);
    
    printf("\nTesting ADD overflow block...\n");
    test_add_overflow(INT_MAX, argc);
    
    return 0;
}
