#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape7() {
    int local[10];
    local[0] = 7;
    return local; // stack escape
}
int main() {
    int *p = escape7();
    return p[0];
}