#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape19() {
    int local[10];
    local[0] = 19;
    return local; // stack escape
}
int main() {
    int *p = escape19();
    return p[0];
}