#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape8() {
    int local = 8;
    return &local;
}
int main() {
    int *p = escape8();
    return 0;
}