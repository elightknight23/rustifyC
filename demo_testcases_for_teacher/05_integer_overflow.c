#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv) {
    // We use 'argc' so the compiler can't pre-calculate the result (constant folding)
    // In real C, max_val + 1 silently wraps around.
    int max_val = INT_MAX - (argc - 1); 
    int overflowed = max_val + 1;
    
    printf("Successfully added: %d\n", overflowed);
    
    // Now we force a real overflow that the compiler can't predict
    printf("Attempting INT_MAX + argc...\n");
    int real_overflow = INT_MAX + argc;
    printf("You will never see this line: %d\n", real_overflow);
    return 0;
}
