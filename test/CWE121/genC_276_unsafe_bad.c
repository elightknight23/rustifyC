#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape1() {
    int local[10];
    local[0] = 1;
    return local; // stack escape
}
int main() {
    int *p = escape1();
    return p[0];
}