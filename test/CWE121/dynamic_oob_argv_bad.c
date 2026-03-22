#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    int arr[10];
    arr[atoi(argv[1])] = 42; // arbitrary runtime access
    return 0;
}
