#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var24;
    if (argc > 10) _var24 = 1;
    printf("%d\n", _var24); // conditionally uninit
    return 0;
}