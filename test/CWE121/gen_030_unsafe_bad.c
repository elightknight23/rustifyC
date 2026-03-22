#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[22];
    arr[30] = 42; // static OOB
    return 0;
}