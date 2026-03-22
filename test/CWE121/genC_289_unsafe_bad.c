#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape14() {
    int local[10];
    local[0] = 14;
    return local; // stack escape
}
int main() {
    int *p = escape14();
    return p[0];
}