#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[5];
    arr[8] = 42; // static OOB
    return 0;
}