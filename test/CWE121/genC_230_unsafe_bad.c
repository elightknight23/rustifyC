#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var5;
    if (argc > 10) _var5 = 1;
    printf("%d\n", _var5); // conditionally uninit
    return 0;
}