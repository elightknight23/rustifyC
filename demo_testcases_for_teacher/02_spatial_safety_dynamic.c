#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int buffer[7];

    // Teacher explanation:
    // Now the index is coming from the user (argv[1]), so the compiler can't know
    // exactly what the value is ahead of time.
    // Usually, C just hopes nothing bad happens. But RustifyC injects a hidden 
    // "if-statement" behind the scenes that checks the bounds right before the code runs!
    int index = atoi(argv[1]);
    
    // If we run this with "./a.out 15", RustifyC will catch it and panic gracefully
    // instead of secretly overwriting other memory.
    buffer[index] = 42;
    printf("Successfully wrote to buffer[%d]. Value is: %d\n", index, buffer[index]);
    return 0;
}
