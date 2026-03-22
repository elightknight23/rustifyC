#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int* escape7() {
    int local = 7;
    return &local;
}
int main() {
    int *p = escape7();
    return 0;
}