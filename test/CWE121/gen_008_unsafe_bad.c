#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[40];
    arr[42] = 42; // static OOB
    return 0;
}