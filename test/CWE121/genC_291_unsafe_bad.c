#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape16() {
    int local[10];
    local[0] = 16;
    return local; // stack escape
}
int main() {
    int *p = escape16();
    return p[0];
}