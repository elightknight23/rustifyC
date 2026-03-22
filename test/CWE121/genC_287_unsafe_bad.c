#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape12() {
    int local[10];
    local[0] = 12;
    return local; // stack escape
}
int main() {
    int *p = escape12();
    return p[0];
}