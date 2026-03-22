#include <stdio.h>

void conditional_init(int flag) {
    int x;
    if (flag) {
        x = 5;
    }
    // "Possibly uninitialized" variable read
    printf("Result: %d\n", x);
}

int main() {
    conditional_init(1);
    return 0;
}
