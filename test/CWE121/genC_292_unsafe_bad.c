#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape17() {
    int local[10];
    local[0] = 17;
    return local; // stack escape
}
int main() {
    int *p = escape17();
    return p[0];
}