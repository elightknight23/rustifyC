#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape23() {
    int local[10];
    local[0] = 23;
    return local; // stack escape
}
int main() {
    int *p = escape23();
    return p[0];
}