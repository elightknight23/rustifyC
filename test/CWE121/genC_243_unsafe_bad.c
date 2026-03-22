#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var18;
    if (argc > 10) _var18 = 1;
    printf("%d\n", _var18); // conditionally uninit
    return 0;
}