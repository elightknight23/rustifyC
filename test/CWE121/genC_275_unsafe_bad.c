#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape0() {
    int local[10];
    local[0] = 0;
    return local; // stack escape
}
int main() {
    int *p = escape0();
    return p[0];
}