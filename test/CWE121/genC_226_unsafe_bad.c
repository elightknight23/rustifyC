#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var1;
    if (argc > 10) _var1 = 1;
    printf("%d\n", _var1); // conditionally uninit
    return 0;
}