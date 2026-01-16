#include <stdio.h>

// Pass index as argument so compiler can't pre-calculate everything
void sensitive_calculation(int index) {
    volatile int buffer[10]; // 'volatile' tells compiler: DO NOT OPTIMIZE THIS
    buffer[index] = 42;      // The write
    
    // The read (prevents dead store elimination)
    printf("Value: %d\n", buffer[index]); 
}

int main() {
    printf("Starting...\n");
    sensitive_calculation(5);
    return 0;
}