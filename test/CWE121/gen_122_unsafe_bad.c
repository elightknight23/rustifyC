#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape6() {
    int local = 6;
    return &local;
}
int main() {
    int *p = escape6();
    return 0;
}