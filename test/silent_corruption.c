#include <stdio.h>

int main() {
    volatile int secret = 1337; // A secret value
    volatile int buffer[2];     // A small array

    printf("Before: Secret is %d\n", secret);

    // ATTACK: Write past the array. 
    // We are writing to index 3 (out of bounds).
    // In memory, 'secret' is sitting right next to 'buffer'.
    buffer[3] = 9999; 

    printf("After:  Secret is %d\n", secret);
    
    return 0;
}
