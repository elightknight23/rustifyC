#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape3() {
    int local = 3;
    return &local;
}
int main() {
    int *p = escape3();
    return 0;
}