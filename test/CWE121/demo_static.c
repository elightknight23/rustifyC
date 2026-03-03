#include <stdio.h>

int main() {
    int buffer[5];
    
    // Valid
    buffer[2] = 10;
    
    // Static overflow
    buffer[10] = 42;
    
    return 0;
}
