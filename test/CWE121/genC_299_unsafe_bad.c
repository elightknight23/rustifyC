#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape24() {
    int local[10];
    local[0] = 24;
    return local; // stack escape
}
int main() {
    int *p = escape24();
    return p[0];
}