#include <stdio.h>

int main(int argc, char **argv) {
    int x = 10 * argc; // safe integer math
    printf("%d\n", x);
    return 0;
}
