// Test: Passing stack address to external function
#include <stdio.h>

// External function (declaration only)
extern void external_fn(int *ptr);

void passes_to_external() {
    int x = 42;
    external_fn(&x);  // WARNING: might escape
}

int main() {
    passes_to_external();
    return 0;
}
