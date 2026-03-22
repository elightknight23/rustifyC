#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int x;
    if (argc > 1) x = 5;
    printf("%d\n", x); // possibly uninitialized
    return 0;
}
