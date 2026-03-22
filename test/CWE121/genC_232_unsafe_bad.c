#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var7;
    if (argc > 10) _var7 = 1;
    printf("%d\n", _var7); // conditionally uninit
    return 0;
}