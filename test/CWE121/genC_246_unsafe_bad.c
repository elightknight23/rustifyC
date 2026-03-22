#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var21;
    if (argc > 10) _var21 = 1;
    printf("%d\n", _var21); // conditionally uninit
    return 0;
}