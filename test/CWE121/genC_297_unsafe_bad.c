#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape22() {
    int local[10];
    local[0] = 22;
    return local; // stack escape
}
int main() {
    int *p = escape22();
    return p[0];
}