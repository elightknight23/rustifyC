#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv) {
    int x = INT_MAX + argc; // INT_MAX + 1 = overflow
    printf("%d\n", x);
    return 0;
}
