#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape1() {
    int local = 1;
    return &local;
}
int main() {
    int *p = escape1();
    return 0;
}