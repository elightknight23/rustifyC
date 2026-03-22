#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[26];
    arr[28] = 42; // static OOB
    return 0;
}