#include <stdio.h>

int* global_ptr;

void bad_func() {
    int x = 42;
    global_ptr = &x; // stack to global escape
}

int main() {
    bad_func();
    return 0;
}
