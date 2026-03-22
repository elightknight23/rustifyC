#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var13;
    if (argc > 10) _var13 = 1;
    printf("%d\n", _var13); // conditionally uninit
    return 0;
}