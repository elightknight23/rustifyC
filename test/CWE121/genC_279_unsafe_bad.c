#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape4() {
    int local[10];
    local[0] = 4;
    return local; // stack escape
}
int main() {
    int *p = escape4();
    return p[0];
}