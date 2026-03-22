#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var14;
    if (argc > 10) _var14 = 1;
    printf("%d\n", _var14); // conditionally uninit
    return 0;
}