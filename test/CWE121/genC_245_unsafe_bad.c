#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var20;
    if (argc > 10) _var20 = 1;
    printf("%d\n", _var20); // conditionally uninit
    return 0;
}