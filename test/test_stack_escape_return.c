// Test: Returning address of local variable
#include <stdio.h>

int* returns_local() {
    int x = 42;
    return &x;  // ERROR: stack escape via return
}

int main() {
    int *p = returns_local();
    printf("%d\n", *p);  // Would be use-after-return
    return 0;
}
