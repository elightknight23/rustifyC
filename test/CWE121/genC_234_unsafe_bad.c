#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var9;
    if (argc > 10) _var9 = 1;
    printf("%d\n", _var9); // conditionally uninit
    return 0;
}