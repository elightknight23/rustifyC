#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape3() {
    int local[10];
    local[0] = 3;
    return local; // stack escape
}
int main() {
    int *p = escape3();
    return p[0];
}