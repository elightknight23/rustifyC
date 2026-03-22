#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[41];
    arr[51] = 42; // static OOB
    return 0;
}