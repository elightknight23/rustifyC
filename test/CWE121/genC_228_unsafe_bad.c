#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var3;
    if (argc > 10) _var3 = 1;
    printf("%d\n", _var3); // conditionally uninit
    return 0;
}