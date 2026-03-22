#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape6() {
    int local[10];
    local[0] = 6;
    return local; // stack escape
}
int main() {
    int *p = escape6();
    return p[0];
}