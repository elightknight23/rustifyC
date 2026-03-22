#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape21() {
    int local[10];
    local[0] = 21;
    return local; // stack escape
}
int main() {
    int *p = escape21();
    return p[0];
}