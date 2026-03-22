#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var0;
    if (argc > 10) _var0 = 1;
    printf("%d\n", _var0); // conditionally uninit
    return 0;
}