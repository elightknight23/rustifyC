#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var22;
    if (argc > 10) _var22 = 1;
    printf("%d\n", _var22); // conditionally uninit
    return 0;
}