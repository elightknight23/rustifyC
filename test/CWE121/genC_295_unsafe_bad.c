#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape20() {
    int local[10];
    local[0] = 20;
    return local; // stack escape
}
int main() {
    int *p = escape20();
    return p[0];
}