#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[9];
    arr[19] = 42; // static OOB
    return 0;
}