#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape5() {
    int local[10];
    local[0] = 5;
    return local; // stack escape
}
int main() {
    int *p = escape5();
    return p[0];
}