#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape18() {
    int local[10];
    local[0] = 18;
    return local; // stack escape
}
int main() {
    int *p = escape18();
    return p[0];
}