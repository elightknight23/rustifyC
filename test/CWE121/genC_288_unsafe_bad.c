#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape13() {
    int local[10];
    local[0] = 13;
    return local; // stack escape
}
int main() {
    int *p = escape13();
    return p[0];
}