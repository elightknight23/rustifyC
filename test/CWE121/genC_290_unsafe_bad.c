#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape15() {
    int local[10];
    local[0] = 15;
    return local; // stack escape
}
int main() {
    int *p = escape15();
    return p[0];
}