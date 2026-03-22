#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var16;
    if (argc > 10) _var16 = 1;
    printf("%d\n", _var16); // conditionally uninit
    return 0;
}