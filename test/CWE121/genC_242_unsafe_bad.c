#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var17;
    if (argc > 10) _var17 = 1;
    printf("%d\n", _var17); // conditionally uninit
    return 0;
}