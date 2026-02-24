#include <stdio.h>
#include <stdlib.h>

// 1. Disable inlining so Clang generates a real function call
// 2. Use 'noinline' attribute
__attribute__((noinline))
void unsafe_access(int index) {
    volatile int buffer[10];
    
    printf("Attempting access at index %d...\n", index);
    
    // RustifyC should see 'index' as a Variable here
    buffer[index] = 42; 
    
    printf("Success! (This should not print)\n");
}

int main(int argc, char** argv) {
    // 3. Get value from runtime args so compiler can't pre-calculate it
    int index = -1;
    if (argc > 1) {
        index = atoi(argv[1]);
    }

    unsafe_access(index);
    return 0;
}