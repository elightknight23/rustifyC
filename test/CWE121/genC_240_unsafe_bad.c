#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var15;
    if (argc > 10) _var15 = 1;
    printf("%d\n", _var15); // conditionally uninit
    return 0;
}