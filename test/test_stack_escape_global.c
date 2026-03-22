// Test: Storing stack address to global
#include <stdio.h>

int *global_ptr;

void stores_to_global() {
    int x = 42;
    global_ptr = &x;  // ERROR: stack escape via global
}

int main() {
    stores_to_global();
    printf("%d\n", *global_ptr);  // Would be use-after-return
    return 0;
}
