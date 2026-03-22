// Test: Cases that should NOT trigger errors
#include <stdio.h>

void safe_printf() {
    int x = 42;
    printf("%d\n", x);  // Safe: passes by value
}

void safe_address_of() {
    int x = 42;
    int *p = &x;
    printf("%d\n", *p);  // Safe: pointer doesn't escape
}

int returns_value() {
    int x = 42;
    return x;  // Safe: returns value, not address
}

int main() {
    safe_printf();
    safe_address_of();
    int y = returns_value();
    return 0;
}
