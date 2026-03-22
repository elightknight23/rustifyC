#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[36];
    arr[43] = 42; // static OOB
    return 0;
}