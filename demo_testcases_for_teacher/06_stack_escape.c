#include <stdio.h>

int* get_dangling_pointer() {
    int local_var = 42;
    // Teacher explanation:
    // 'local_var' is destroyed the moment this function finishes.
    // But we are returning its memory address back to the main function!
    // This creates what's called a "Dangling Pointer". 
    // RustifyC detects that a local stack pointer is trying to "escape" 
    // the function it was born in, and blocks the compilation to protect temporal safety!
    return &local_var; 
}

int main() {
    int* bad_ptr = get_dangling_pointer();
    printf("Value: %d\n", *bad_ptr);
    return 0;
}
