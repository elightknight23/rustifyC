#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[37];
    arr[47] = 42; // static OOB
    return 0;
}