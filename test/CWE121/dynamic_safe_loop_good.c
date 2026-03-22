#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int arr[10];
    for (int i = 0; i < 10; ++i) { // within bounds loop
        arr[i] = i;
    }
    return 0;
}
