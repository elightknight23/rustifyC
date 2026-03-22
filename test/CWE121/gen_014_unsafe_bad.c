#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[48];
    arr[50] = 42; // static OOB
    return 0;
}