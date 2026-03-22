#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int _var23;
    if (argc > 10) _var23 = 1;
    printf("%d\n", _var23); // conditionally uninit
    return 0;
}