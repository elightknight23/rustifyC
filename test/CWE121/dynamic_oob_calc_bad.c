#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int arr[5];
    int calculated = argc * 10;
    arr[calculated] = 42; // math derived access
    return 0;
}
