#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int x = INT_MAX - 6 + argc + 50; // overflow
    printf("%d\n", x);
    return 0;
}