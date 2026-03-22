#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape10() {
    int local[10];
    local[0] = 10;
    return local; // stack escape
}
int main() {
    int *p = escape10();
    return p[0];
}