#include <stdio.h>
#include <limits.h>

int main(int argc, char **argv) {
    int x = (INT_MAX / 2) * (argc + 2); // Guaranteed overflow with default args (argc=1)
    printf("%d\n", x);
    return 0;
}
