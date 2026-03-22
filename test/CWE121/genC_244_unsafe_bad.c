#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var19;
    if (argc > 10) _var19 = 1;
    printf("%d\n", _var19); // conditionally uninit
    return 0;
}