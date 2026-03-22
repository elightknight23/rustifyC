#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var6;
    if (argc > 10) _var6 = 1;
    printf("%d\n", _var6); // conditionally uninit
    return 0;
}