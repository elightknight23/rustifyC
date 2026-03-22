#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[14];
    arr[17] = 42; // static OOB
    return 0;
}