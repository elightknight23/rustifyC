#include <stdio.h>

int* bad_func() {
    int x = 42;
    return &x; // stack escape
}

int main() {
    int *p = bad_func();
    return 0;
}
