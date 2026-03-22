#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape11() {
    int local[10];
    local[0] = 11;
    return local; // stack escape
}
int main() {
    int *p = escape11();
    return p[0];
}