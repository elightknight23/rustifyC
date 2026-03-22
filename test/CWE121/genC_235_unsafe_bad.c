#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var10;
    if (argc > 10) _var10 = 1;
    printf("%d\n", _var10); // conditionally uninit
    return 0;
}