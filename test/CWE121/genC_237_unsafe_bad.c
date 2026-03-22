#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var12;
    if (argc > 10) _var12 = 1;
    printf("%d\n", _var12); // conditionally uninit
    return 0;
}