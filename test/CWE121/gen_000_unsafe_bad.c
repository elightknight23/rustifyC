#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[20];
    arr[26] = 42; // static OOB
    return 0;
}