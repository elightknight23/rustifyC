#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape9() {
    int local[10];
    local[0] = 9;
    return local; // stack escape
}
int main() {
    int *p = escape9();
    return p[0];
}