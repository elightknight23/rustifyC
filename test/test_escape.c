#include <stdio.h>
int* returns_local() { int x = 42; return &x; }
void safe_func() { int x=42; printf("%d\n", x); }
int main() { return 0; }
