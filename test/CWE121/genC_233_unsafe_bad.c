#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var8;
    if (argc > 10) _var8 = 1;
    printf("%d\n", _var8); // conditionally uninit
    return 0;
}