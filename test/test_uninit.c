#include <stdio.h>
#include <stdlib.h>

void definite_bug() {
    int x;
    // x is NEVER initialized before this read, should PANIC
    printf("Definite Bug: %d\n", x);  
}

void init_then_use() {
    int x = 5;
    // initialized! safe.
    printf("Safe Use: %d\n", x);  
}

void conditional_init(int rand_val) {
    int x;
    if (rand_val > 5) {
        x = 5;
    }
    // we may or may not have initialized x. 
    // Since our analysis is conservative, it's fine if it injects a check/fails here.
    printf("Conditional Use: %d\n", x);  
}

int main(int argc, char** argv) {
    init_then_use();
    printf("init_then_use() passed safely.\n");

    // Let's call the definite bug to trigger our runtime panic
    definite_bug();
    
    return 0;
}
