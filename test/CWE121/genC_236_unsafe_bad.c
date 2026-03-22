#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var11;
    if (argc > 10) _var11 = 1;
    printf("%d\n", _var11); // conditionally uninit
    return 0;
}