#include <stdio.h>
#include <stdlib.h>

void process(int index) {
    int arr[10];
    arr[index] = 42;
    printf("Write successful to index %d\n", index);
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    process(atoi(argv[1]));
    return 0;
}
