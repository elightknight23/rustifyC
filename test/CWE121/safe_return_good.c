#include <stdio.h>

int safe_func() {
    int x = 42;
    return x; // safe by-value return
}

int main() {
    int val = safe_func();
    return 0;
}
