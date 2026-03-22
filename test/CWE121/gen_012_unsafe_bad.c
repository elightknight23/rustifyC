#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[6];
    arr[14] = 42; // static OOB
    return 0;
}