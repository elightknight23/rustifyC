#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[8];
    arr[13] = 42; // static OOB
    return 0;
}