#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var2;
    if (argc > 10) _var2 = 1;
    printf("%d\n", _var2); // conditionally uninit
    return 0;
}