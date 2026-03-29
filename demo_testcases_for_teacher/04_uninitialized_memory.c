#include <stdio.h>

int main() {
    int secret_variable;
    
    // Teacher explanation:
    // Here we forgot to give 'secret_variable' a starting value (we didn't initialize it).
    // In C, whatever old garbage data was sitting there in memory will be printed. 
    // This often leaks sensitive data (like old passwords) from previous programs!
    // RustifyC tracks the "Def-Use" chain, sees we read before we write, 
    // and injects a trap to stop the program securely.
    printf("Uninitialized memory leak: %d\n", secret_variable);
    
    return 0;
}
