#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape8() {
    int local[10];
    local[0] = 8;
    return local; // stack escape
}
int main() {
    int *p = escape8();
    return p[0];
}