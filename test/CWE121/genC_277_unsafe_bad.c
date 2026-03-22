#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape2() {
    int local[10];
    local[0] = 2;
    return local; // stack escape
}
int main() {
    int *p = escape2();
    return p[0];
}