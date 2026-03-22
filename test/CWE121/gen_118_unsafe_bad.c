#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape4() {
    int local = 4;
    return &local;
}
int main() {
    int *p = escape4();
    return 0;
}