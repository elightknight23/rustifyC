#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape9() {
    int local = 9;
    return &local;
}
int main() {
    int *p = escape9();
    return 0;
}