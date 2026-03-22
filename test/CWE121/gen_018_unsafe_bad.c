#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[42];
    arr[49] = 42; // static OOB
    return 0;
}