#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var4;
    if (argc > 10) _var4 = 1;
    printf("%d\n", _var4); // conditionally uninit
    return 0;
}