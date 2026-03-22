#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[10];
    arr[11] = 42; // static OOB
    return 0;
}