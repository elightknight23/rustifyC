#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int x = 1000 + 4 * argc; // safe
    printf("%d\n", x);
    return 0;
}